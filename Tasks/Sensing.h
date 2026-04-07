// SENSING_H
#pragma once

#include <Arduino.h>
#include "TCRT5000.h"

void irSensing(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(20);
  TickType_t lastWakeTime = xTaskGetTickCount();

  TCRT5000 left(A3), right(A1);

  Serial.println("Started ir sensing");

  while(true){
    xTaskDelayUntil(&lastWakeTime, period);


    if(xSemaphoreTake(irSemaphore, pdMS_TO_TICKS(5)) == pdTRUE){
      // Serial.println("Sensed");
      sensors.left = left.read();
      sensors.right = right.read();

      // Serial.print("Left = "); Serial.println(sensors.left);
      // Serial.print("Right = "); Serial.println(sensors.right);

      xSemaphoreGive(irSemaphore);
    }
    

  }
}