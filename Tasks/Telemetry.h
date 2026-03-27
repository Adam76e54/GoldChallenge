#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "GlobalObjects.h"
#include "Buffer.h"
#include "Commands.h"
#include "NetworkSetup.h"
#include "GUI.h"

void telemetry(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(500);
  TickType_t lastWakeTime = xTaskGetTickCount();

  WiFiClient GUI;
  WiFiServer server;

  Buffer<128> in;

  server.begin(wifi::PORT);
  wifi::initialiseAccessPoint();

  while(true){
    keep(GUI, server);

    read(GUI, in);

    enqueue(in, commandQueue);


    xTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(period));
  }
}
