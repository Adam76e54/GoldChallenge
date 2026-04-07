// MOVE_H
#pragma once

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "Controller.h"
#include "L293D.h"
#include "HCSR04.h"
#include "GlobalObjects.h"
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

  // Set up controllers
  Controller rightSpeedController(0,0,0), leftSpeedController(0,0,0);
  access(speedPIDSemaphore, pdMS_TO_TICKS(50), 
    [&rightSpeedController, &leftSpeedController](){
      rightSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
      leftSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
      // Serial.print("Left and right kp = "); Serial.println(speedCoefficients.kp);
    }
  );

  // Reset counts
  leftEncoder.reset(); rightEncoder.reset();

  constexpr uint8_t MAX_CMS = 40; // For initial guess at speeds


  uint16_t leftThreshold = 0, rightThreshold = 0;
  access(thresholdSemaphore, pdMS_TO_TICKS(50), [&leftThreshold, &rightThreshold](){
    leftThreshold = thresholds.left;
    rightThreshold = thresholds.right;
  });

  // Adjust targetAngularVelocity
  float angleKp = 0;
  access(angleSemaphore, pdMS_TO_TICKS(50), [&angleKp](){
    angleKp = angleCoefficients.kp;
    // Serial.print("Angle kp = "); Serial.println(angleKp);
  });

  uint8_t targetsIdx = 0, actualsIdx = 0;
  uint16_t currentTargetSpeed_cms = targetSpeeds[targetsIdx];
  Serial.print("Target velocity = "); Serial.println(currentTargetSpeed_cms);
  unsigned long currentChangeTime_us = (unsigned long)targetTimes[targetsIdx] * 1e6;
  Serial.print("Target change time = "); Serial.println(currentChangeTime_us);

  float leftPercentage = currentTargetSpeed_cms / (float) MAX_CMS;
  float rightPercentage = currentTargetSpeed_cms / (float) MAX_CMS;

  constexpr unsigned long MAX_TIME_us = 60 * 1e6;
  constexpr unsigned long SAMPLE_RATE_us = 8 * 1e5;
  const auto start = micros();

  auto lastSample = start;
  unsigned long lastLeftCount = 0, lastRightCount = 0;

  float leftMeasurement_cms = 0, rightMeasurement_cms = 0;
  bool stopped = false;
  
  bool timeout = false;
  while(!timeout && !stopped){
    // Block periodically, this is a bit janky but i'd need a big refactor if I want it smoother.
    vTaskDelay(2);

    // Serial.println("Going forward:");

    auto currentTime_us = timeSince_us(start);
    // Serial.print("Current time = "); Serial.println(currentTime_us);
    if(currentTime_us >= MAX_TIME_us){
      Serial.println("Timed out");
      timeout = true;
    }

    if(currentTime_us >= currentChangeTime_us && (targetsIdx + 1) < data.ARRAY_SIZE){
      ++targetsIdx;
      // Serial.print("Idx = "); Serial.println(idx);
      currentChangeTime_us += (unsigned long)targetTimes[targetsIdx] * 1e6;
      currentTargetSpeed_cms = targetSpeeds[targetsIdx];
    
      Serial.print("Target velocity = "); Serial.println(currentTargetSpeed_cms);
      Serial.print("Target change time = "); Serial.println(currentChangeTime_us);


    }

    // Fetch if stopped
    access(stoppedSemaphore, pdMS_TO_TICKS(5), [&stopped](){stopped = state.stopped;});


    if(safe(ears)){
      // Serial.println("Safe");
      // Serial.print("Left and right percentages = "); Serial.print(leftPercentage);
      // Serial.print(", ");Serial.println(rightPercentage);

      driver.forward(leftPercentage, rightPercentage);

      if(auto interval = timeSince_us(lastSample); interval >= SAMPLE_RATE_us){
        //  - MEAURE SPEEDS -

        // Note we could make the tighter by having task notifcation 
        // from the ISRs so we measure the exact time between counts
        auto leftCount = leftEncoder.count();
        auto rightCount = rightEncoder.count();

        auto leftChange = leftCount - lastLeftCount;
        auto rightChange = rightCount - lastRightCount;

        constexpr float CIRCUMFERENCE = 21.0f;

        leftMeasurement_cms = (leftChange / static_cast<float>(interval * 1e-6)) 
                              * CIRCUMFERENCE / leftEncoder.COUNTS_PER_REV_;

        rightMeasurement_cms = (rightChange / static_cast<float>(interval * 1e-6)) 
                               * CIRCUMFERENCE / rightEncoder.COUNTS_PER_REV_;

        // Notify telemetry to send data.
        float velocity = (leftMeasurement_cms + rightMeasurement_cms) / 2.0f;
        access(arraySemaphore, pdMS_TO_TICKS(5), [&velocity, &currentTime_us](){
          data.currentActualSpeed = velocity;
          data.currentActualTime = currentTime_us * 1e-6;
        });
        xTaskNotify(telemetryTaskHandle, actualsIdx, eSetValueWithoutOverwrite);
        ++actualsIdx;


        lastSample += interval;
        lastLeftCount = leftCount;
        lastRightCount = rightCount;

        // - PID ADJUSTMENT -
        constexpr float SCALAR = 0.0001; // This is just to make the PID numbers bigger
        float leftError = (currentTargetSpeed_cms - leftMeasurement_cms) * SCALAR;
        float rightError = (currentTargetSpeed_cms - rightMeasurement_cms) * SCALAR;
        Serial.print("Left and right errors = "); Serial.print(leftError);
        Serial.print(", ");Serial.println(rightError);

        float leftAdjustment = leftSpeedController.PID(leftError);
        float rightAdjustement = rightSpeedController.PID(rightError);
        Serial.print("Left and right adjustments = "); Serial.print(leftAdjustment);
        Serial.print(", ");Serial.println(rightAdjustement);

        leftPercentage += leftAdjustment;
        rightPercentage += rightAdjustement;
      }

      // - LINE FOLLOWING - 
      bool leftOnLine = false, rightOnLine = false;
      access(irSemaphore, pdMS_TO_TICKS(5), [&leftOnLine, &rightOnLine, leftThreshold, rightThreshold]() {
        leftOnLine = sensors.left < leftThreshold;
        rightOnLine = sensors.right < rightThreshold;
        Serial.println("Caught the sensor data");
      });


      leftPercentage = constrain(leftPercentage, 0.0f, 1.0f);
      rightPercentage = constrain(rightPercentage, 0.0f, 1.0f);

      if(rightOnLine && !leftOnLine){
        Serial.println("Right on line");
        driver.forward(leftPercentage * (1.0f - angleKp), rightPercentage * (1.0f + angleKp));
      } else if(leftOnLine && !rightOnLine){
        Serial.println("left on line");
        driver.forward(leftPercentage * (1.0f + angleKp), rightPercentage * (1.0f - angleKp));
      } else {
        // Both on or both off — go straight
        Serial.println("Default");
        Serial.print("Left and right thresholds: "); Serial.print(leftThreshold); Serial.print(" "); Serial.println(rightThreshold);
        driver.forward(leftPercentage, rightPercentage);
      }

    } else {
      // Serial.println("Unsafe");
      driver.brake(L293D_BRAKE_TIME);
    }


  }

  access(stoppedSemaphore, pdMS_TO_TICKS(50), [](){
    state.stopped = true;
  });

  Serial.println("Finished forward");

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