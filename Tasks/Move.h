// MOVE_H
#pragma once

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "Controller.h"
#include "L293D.h"
#include "HCSR04.h"
#include "GlobalObjects.h"
#include "ISR.h"

void forward(L293D& driver, HCSR04& ears, 
             uint8_t targetSpeed[], uint8_t targetTime[], uint8_t actualSpeed[], uint8_t actualTimes[]);
        
bool safe(HCSR04 &ears);
unsigned long timeSince_us(unsigned long start);

void move(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(100);
  TickType_t lastWakeTime = xTaskGetTickCount();

  L293D driver(6, 7, 11, 12, 9, 10);

  HCSR04 ears(8, A5);

  Serial.println("Started move task");

  // Local copies of data.arrays
  uint8_t targetSpeeds[data.ARRAY_SIZE], targetTimes[data.ARRAY_SIZE];
  uint8_t actualSpeeds[data.MEASURING_ARRAY_SIZE], actualTimes[data.MEASURING_ARRAY_SIZE];

  // How long to wait for Semaphores
  constexpr uint16_t WAIT_TIME_MS = 50;


  while(true){
    xTaskDelayUntil(&lastWakeTime, period);


    Serial.println("Inside move task");
    
    // Make non volatile local copies of the data, just for clarity
    uint16_t left = sensors.left;
    uint16_t right = sensors.right;



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
      forward(driver, ears, targetTimes, targetSpeeds, actualSpeeds, actualTimes);
    } else {
      driver.brake(L293D_BRAKE_TIME);
    }
  }
  
}

void forward(L293D& driver, HCSR04& ears, 
             uint8_t targetSpeeds[], uint8_t targetTimes[], uint8_t actualSpeed[], uint8_t actualTimes[]){
  // Set up controllers
  Controller rightSpeedController(0,0,0), leftSpeedController(0,0,0);
  access(speedPIDSemaphore, pdMS_TO_TICKS(50), 
    [&rightSpeedController, &leftSpeedController](){
      rightSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
      leftSpeedController.set(speedCoefficients.kp, speedCoefficients.ki, speedCoefficients.kd);
    }
  );

  // Reset counts
  leftEncoder.reset(); rightEncoder.reset();

  constexpr uint8_t MAX_CMS = 50; // For initial guess at speeds

  uint8_t idx = 0;
  
  unsigned int currentTargetSpeed = targetSpeeds[idx];
  unsigned int changeTime = targetTimes[idx];

  float leftPercentage = currentTargetSpeed / (float) MAX_CMS;
  float rightPercentage = currentTargetSpeed / (float) MAX_CMS;


  constexpr unsigned long MAX_TIME = 60 * 1e6;
  constexpr unsigned long SAMPLE_RATE = 1 * 1e6;
  const auto start = micros();

  auto lastSample = start;
  unsigned long lastLeftCount = 0, lastRightCount = 0;

  float leftMeasurement_cms = 0, rightMeasurement_cms = 0;
  uint8_t idx = 0;
  bool stopped = false;
  
  bool timeout = false;
  while(!timeout && !stopped){

    auto currentTime = timeSince_us(start);
    if(currentTime >= MAX_TIME){
      timeout = true;
    }

    if(currentTime > changeTime && idx < data.ARRAY_SIZE){
      ++idx;
      changeTime = targetTimes[idx];
      currentTargetSpeed = targetSpeeds[idx];
    }

    // Fetch if stopped
    access(stoppedSemaphore, pdMS_TO_TICKS(5), [&stopped](){stopped = state.stopped;});


    if(safe(ears)){
      driver.forward(leftPercentage, rightPercentage);

      // - MEASURE SPEEDS -
      if(auto interval = timeSince_us(lastSample); interval >= SAMPLE_RATE){
        //  - MEAURE SPEEDS -
        auto leftCount = leftEncoder.count();
        auto rightCount = rightEncoder.count();

        auto leftChange = leftCount - lastLeftCount;
        auto rightChange = rightCount - lastRightCount;

        constexpr float CIRCUMFERENCE = 21.0f;

        leftMeasurement_cms = (leftChange / static_cast<float>(interval * 1e-6)) 
                              * CIRCUMFERENCE / leftEncoder.COUNTS_PER_REV_;

        rightMeasurement_cms = (rightChange / static_cast<float>(interval * 1e-6)) 
                               * CIRCUMFERENCE / rightEncoder.COUNTS_PER_REV_;

        lastSample += interval;
      }

      // - ANGULAR VELOCITY AND TARGET SPEEDS COMPUTATION - 
      constexpr float AXLE = 14.0f;
      float velocity = (leftMeasurement_cms + rightMeasurement_cms) / 2.0f;

      static uint16_t leftThreshold = 0, rightThreshold = 0;
      access(thresholdSemaphore, pdMS_TO_TICKS(5), [](){
        leftThreshold = thresholds.left;
        rightThreshold = thresholds.right;
      });

      bool leftOnLine = false, rightOnLine = false;
      access(irSemaphore, pdMS_TO_TICKS(5), [&leftOnLine, &rightOnLine]() {
        leftOnLine = sensors.left < leftThreshold;
        rightOnLine = sensors.right < rightThreshold;
      });

      // Counters for estimating target angular velocity
      static unsigned long rightIrCounter = 0, leftIrCounter = 0;
      rightIrCounter = rightOnLine ? rightIrCounter + 1 : 0;
      leftIrCounter = leftOnLine ? leftIrCounter + 1 : 0;

      // Adjust targetAngularVelocity
      float targetAngularVelocity = 0;
      access(angleSemaphore, pdMS_TO_TICKS(5), [&]{
        targetAngularVelocity = (rightIrCounter * rightIrCounter) * angleCoefficients.kp
                                 - (leftIrCounter * leftIrCounter) * angleCoefficients.kp;
      });

      // Set left and right target speeds based on angular velocity target
      float targetLeftSpeed = currentTargetSpeed - (targetAngularVelocity * AXLE / 2.0f);
      float targetRightSpeed = currentTargetSpeed + (targetAngularVelocity * AXLE / 2.0f);

      // - PID ADJUSTMENT -
      float leftError = targetLeftSpeed - leftMeasurement_cms;
      float rightError = targetRightSpeed - rightMeasurement_cms;

      float leftAdjustment = leftSpeedController.PID(leftError);
      float rightAdjustement = rightSpeedController.PID(rightError);

      leftPercentage += leftAdjustment;
      rightPercentage += rightAdjustement;

      driver.forward(leftPercentage, rightPercentage);

    } else {
      driver.brake(L293D_BRAKE_TIME);
      break;
    }

    // Block the task for a tick to avoid starving telemetry? Not sure it this will work. 
    vTaskDelay(1);
  }


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