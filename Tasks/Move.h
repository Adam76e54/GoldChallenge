// MOVE_H
#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "HCSR04.h"
#include "GlobalObjects.h"
#include <Arduino_FreeRTOS.h>

void move(void*){
  L293D driver(6, 7, 11, 12, 9, 10);

  HCSR04 ears(8, A5);
  Serial.println("Started move task");

  while(true){
    // Block until we get a notification from the sensors
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Make non volatile local copies of the data, just for clarity
    uint16_t left = sensors.left;
    uint16_t right = sensors.right;

    bool stopped = false;
    if(xSemaphoreTake(stoppedSemaphore, pdMS_TO_TICKS(50))){
      stopped = state.stopped;
      xSemaphoreGive(stoppedSemaphore);
    }
    
    if(!stopped){
      Serial.println("Not stopped");
    }
  }
  
}