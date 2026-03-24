#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "State.h"
#include "Buffer.h"
#include "Commands.h"
#include "NetworkSetup.h"

void telemetry(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(500);
  TickType_t lastWakeTime = xTaskGetTickCount();

  WiFiClient GUI;
  WiFiServer server;

  Buffer<200> in;

  server.begin(wifi::PORT);
  wifi::initialiseAccessPoint();

  while(true){
    keep(GUI, server);

    read(GUI, in);

    handle(in);


    xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(period));
  }
}
