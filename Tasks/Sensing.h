// SENSING_H
#pragma once

#include <Arduino.h>
#include "TCRT5000.h"

void irSensing(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(20);
  TickType_t lastWakeTime = xTaskGetTickCount();

  TCRT5000 left(1), right(2);

  Serial.println("Started ir sensing");

  while(true){
    xTaskDelayUntil(&lastWakeTime, period);

    // Serial.println("Sensed");
    sensors.left = left.read();
    sensors.right = right.read();

    if(moveTaskHandle != nullptr){
      xTaskNotifyGive(moveTaskHandle);
    }

  }
}