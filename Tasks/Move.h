// MOVE_H
#pragma once

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "Controller.h"
#include "L293D.h"
#include "HCSR04.h"
#include "GlobalObjects.h"
#include "ROB12629.h"
#include "ISR.h"


ROB12629 leftEncoder(2), rightEncoder(3);

void forward(L293D& driver, HCSR04& ears, 
             uint8_t targetSpeed[], uint8_t targetTime[]);
        
bool safe(HCSR04 &ears);
unsigned long timeSince_us(unsigned long start);

void decide(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(100);
  TickType_t lastWakeTime = xTaskGetTickCount();

  L293D driver(6, 7, 11, 12, 9, 10);
  driver.begin();

  HCSR04 ears(8, A5);
  ears.begin();

  leftEncoder.begin(leftISR);
  rightEncoder.begin(rightISR);

  Serial.println("Started move task");

  // Local copies of data.arrays
  uint8_t targetSpeeds[data.ARRAY_SIZE], targetTimes[data.ARRAY_SIZE];

  // How long to wait for Semaphores
  constexpr uint16_t WAIT_TIME_MS = 50;


  while(true){
    xTaskDelayUntil(&lastWakeTime, period);

    // Fetch targets if changed
    if(xSemaphoreTake(arraySemaphore, pdMS_TO_TICKS(WAIT_TIME_MS)) == pdTRUE){

      if(data.changed){
        for(uint8_t i = 0; i < data.ARRAY_SIZE; ++i){
          targetSpeeds[i] = data.targetSpeeds[i];
          targetTimes[i] = data.targetTimes[i];
          data.changed = false;
        }
      }

      xSemaphoreGive(arraySemaphore);
    }

    // Fetch stopped
    bool stopped = false;
    access(stoppedSemaphore, pdMS_TO_TICKS(5), [&stopped](){stopped = state.stopped;});

    if(!stopped){
      forward(driver, ears, targetSpeeds, targetTimes);
    } else {
      driver.brake(L293D_BRAKE_TIME);
    }
  }
  
}

void forward(L293D& driver, HCSR04& ears, 
             uint8_t targetSpeeds[], uint8_t targetTimes[]){

  // constexpr TickType_t period = pdMS_TO_TICKS(5);
  // TickType_t lastWakeTime = xTaskGetTickCount();

  // Set up controllers, turning factor, IR thresholds
  Controller rightSpeedController(0,0,0), leftSpeedController(0,0,0);
  access(speedPIDSemaphore, pdMS_TO_TICKS(50), 
    [&rightSpeedController, &leftSpeedController](){
      rightSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
      leftSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
      // Serial.print("Left and right kp = "); Serial.println(speedCoefficients.kp);
    }
  );
  float angleKp = 0;
  access(angleSemaphore, pdMS_TO_TICKS(50), [&angleKp]{
    angleKp = angleCoefficients.kp;
  });
  uint16_t leftThreshold = 0, rightThreshold = 0;
  access(thresholdSemaphore, pdMS_TO_TICKS(50), [&leftThreshold, &rightThreshold](){
    leftThreshold = thresholds.left;
    rightThreshold = thresholds.right;
  });

  // Reset counts
  leftEncoder.reset(); rightEncoder.reset();

  constexpr uint8_t MAX_CMS = 50; // For initial guess at speeds
  constexpr float CIRCUMFERENCE = 21.0f;
  constexpr float CM_PER_COUNT = CIRCUMFERENCE / leftEncoder.COUNTS_PER_REV_;
  // constexpr float AXLE = 13.5f;

  uint8_t targetsIdx = 0, actualsIdx = 0;
  uint16_t currentTargetSpeed = targetSpeeds[targetsIdx];
  // Serial.print("Target velocity = "); Serial.println(currentTargetSpeed);
  unsigned long currentChangeTime_us = (unsigned long)targetTimes[targetsIdx] * 1e6;
  // Serial.print("Target change time = "); Serial.println(currentChangeTime_us);

  float leftPercentage = currentTargetSpeed / (float) MAX_CMS;
  float rightPercentage = currentTargetSpeed / (float) MAX_CMS;

  constexpr unsigned long MAX_TIME_us = 60 * 1e6;
  constexpr unsigned long SAMPLE_RATE_us = 5 * 1e5;
  const auto start = micros();

  unsigned long lastLeftCount = 0, lastRightCount = 0;

  float leftMeasurement_cms = 0, rightMeasurement_cms = 0;

  unsigned long lastSample = micros();
  
  // unsigned long rightIrCounter = 0, leftIrCounter = 0;
  unsigned long rightEncoderCounter = 0, leftEncoderCounter = 0;

  bool stopped = false;
  access(stoppedSemaphore, pdMS_TO_TICKS(50), [&stopped](){stopped = state.stopped;});
  bool timeout = false;
  while(!timeout && !stopped){
    // Block periodically, this is a bit janky but i'd need a big refactor if I want it smoother.
    // Serial.println("Going forward:");
    vTaskDelay(2);

    // 1. Update loop conditions
    access(stoppedSemaphore, pdMS_TO_TICKS(5), [&stopped](){stopped = state.stopped;});
    auto currentTime_us = timeSince_us(start);
    if(currentTime_us >= MAX_TIME_us){
      // Serial.println("Timed out");
      timeout = true;
    }

    // 2. Update target speeds and times
    if(currentTime_us >= currentChangeTime_us && (targetsIdx + 1) < data.ARRAY_SIZE){
      ++targetsIdx;
      // Serial.print("Idx = "); Serial.println(idx);
      currentChangeTime_us += (unsigned long)targetTimes[targetsIdx] * 1e6;
      currentTargetSpeed = targetSpeeds[targetsIdx];
    
      // Serial.print("Target velocity = "); Serial.println(currentTargetSpeed);
      // Serial.print("Target change time = "); Serial.println(currentChangeTime_us);
    }

    if(safe(ears)){
      // - RUN FORWARD -

      // Serial.println("Safe");
      // Serial.print("Left and right percentages = "); Serial.print(leftPercentage);
      // Serial.print(", ");Serial.println(rightPercentage);

      driver.forward(leftPercentage, rightPercentage);

      // 1. FOLLOW LINE
      bool leftOnLine = false, rightOnLine = false;
      access(irSemaphore, pdMS_TO_TICKS(5), [&leftOnLine, &rightOnLine, leftThreshold, rightThreshold]() {
        leftOnLine = sensors.left < leftThreshold ;

        rightOnLine = sensors.right < rightThreshold;

        // Serial.print("Sensors.right = "); Serial.println(sensors.right);
        // Serial.print("Right threshold = "); Serial.println(rightThreshold);
      });

      // Set left and right target speeds based on angular velocity target
      float targetLeftSpeed = currentTargetSpeed, targetRightSpeed = currentTargetSpeed;

      if(rightOnLine){
        targetLeftSpeed += currentTargetSpeed * angleKp;
        targetRightSpeed -= currentTargetSpeed * angleKp;
      } 

      if(leftOnLine){
        targetLeftSpeed -= currentTargetSpeed * angleKp;
        targetRightSpeed += currentTargetSpeed * angleKp;
      }

      Serial.print("Left target speed = "); Serial.println(targetLeftSpeed);
      Serial.print("Right target speed = "); Serial.println(targetRightSpeed);

      // - 2. MEAUSURE SPEED -> PID ADJUSTMENT -
      leftEncoderCounter = leftEncoder.count();
      rightEncoderCounter = rightEncoder.count();
      
      constexpr float SCALAR = 0.0001; // This is just to make the PID numbers bigger
      constexpr uint8_t COUNTS_TO_USE = 1;
      if(leftEncoderCounter- lastLeftCount == COUNTS_TO_USE){
        auto time_us = micros() - leftEncoder.lastEdgeTime();
        float countChange = leftEncoderCounter - lastLeftCount;

        leftMeasurement_cms = (countChange / (time_us * 1e-6))
                              * (COUNTS_TO_USE * CM_PER_COUNT);

        float error = (targetLeftSpeed - leftMeasurement_cms) * SCALAR;
        float leftAdjustment = leftSpeedController.PID(error);

        leftPercentage += leftAdjustment;
        lastLeftCount = leftEncoderCounter;
      }

      if(rightEncoderCounter - lastRightCount == COUNTS_TO_USE){
        auto time_us = micros() - rightEncoder.lastEdgeTime();
        float countChange = rightEncoderCounter - lastRightCount;

        rightMeasurement_cms = (countChange / (time_us * 1e-6))
                              * (COUNTS_TO_USE * CM_PER_COUNT);

        float error = (targetRightSpeed - rightMeasurement_cms) * SCALAR;
        float rightAdustment = rightSpeedController.PID(error);
        
        rightPercentage += rightAdustment;
        lastRightCount = rightEncoderCounter;
      }


      if(auto interval = timeSince_us(lastSample); interval >= SAMPLE_RATE_us){
        // Notify telemetry to send data.
        float velocity = (leftMeasurement_cms + rightMeasurement_cms) / 2.0f;
        access(arraySemaphore, pdMS_TO_TICKS(5), [velocity, currentTime_us](){
          data.currentActualSpeed = velocity;
          data.currentActualTime = currentTime_us;
        });
        xTaskNotify(telemetryTaskHandle, actualsIdx, eSetValueWithoutOverwrite);
        ++actualsIdx;

        lastSample += interval;
      }

    } else {
      // Serial.println("Unsafe");
      driver.brake(L293D_BRAKE_TIME);
      break;
    }


  }

  access(stoppedSemaphore, pdMS_TO_TICKS(50), [](){
    state.stopped = true;
  });

}



bool safe(HCSR04 &ears){
  constexpr uint8_t US_POLLING_RATE = 100;
  static uint8_t closeCount = 0, farCount = 0;
  constexpr uint8_t MAX_CLOSE_COUNT = 5;
  constexpr uint8_t MAX_FAR_COUNT = 5; 

  auto distance = ears.poll(US_POLLING_RATE);

  //sanity check to filter dodgy sensor readings
  if(distance <= 30){

    if(closeCount < MAX_CLOSE_COUNT){
      ++closeCount;
    } 
    farCount = 0;
    
  } else {

    if(farCount < MAX_FAR_COUNT){
      ++farCount;
    }
    closeCount = 0;
  }

  if(closeCount >= MAX_CLOSE_COUNT){
    return false;
  } else if(farCount >= MAX_FAR_COUNT){
    return true;
  }

  return true;
}

unsigned long timeSince_us(unsigned long start){
  return micros() - start;
}

