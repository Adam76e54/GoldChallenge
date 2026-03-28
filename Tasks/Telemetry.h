// TELEMETRY_H
#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "GlobalObjects.h"
#include "Buffer.h"
#include "Commands.h"
#include "NetworkSetup.h"
#include "Arduino_FreeRTOS.h"

constexpr uint8_t N = 128;

void keep(WiFiClient& GUI, WiFiServer& server);

template <uint8_t N> 
bool read(WiFiClient& GUI, Buffer<N>& buffer);

template <uint8_t N>
void enqueue(Buffer<N>& buffer, QueueHandle_t queue);

void telemetry(void*){
  constexpr TickType_t period = pdMS_TO_TICKS(100);
  TickType_t lastWakeTime = xTaskGetTickCount();

  WiFiClient GUI;
  WiFiServer server;

  Buffer<128> in;

  server.begin(wifi::PORT);
  wifi::initialiseAccessPoint();

  while(true){
    xTaskDelayUntil(&lastWakeTime, period);


    keep(GUI, server);

    // Serial.println("Inside telemetry");
    // Serial.println(GUI.available());

    read(GUI, in);

    enqueue(in, commandQueue);
  }
}

// Keep connection alive
void keep(WiFiClient& GUI, WiFiServer& server){
  if (!GUI || !GUI.connected()) {
    if (GUI) GUI.stop();
    GUI = server.available();
  }
}

//read command into buffer
template <uint8_t N> 
bool read(WiFiClient& GUI, Buffer<N>& buffer){
  if(GUI && GUI.connected()){
    while(GUI.available() > 0){
      byte commandByte = GUI.read();
      buffer.write(commandByte);
      if(commandByte == '\n')break;
    }
  }
  return false;
}

template <uint8_t N>
void enqueue(Buffer<N>& buffer, QueueHandle_t queue){

  if(buffer.available() > 0){
    constexpr unsigned int WAIT_TIME = 100;

    char message [N];

    const uint8_t messageSize = buffer.read(message, N);

    Serial.println(message);
    
    // Get function opcode as a char
    char function = message[0];
    // Get a value if it exists (we take a pointer to the rest of the string)
    const char* value = (messageSize > 2) ? &message[2] : nullptr;

    // Serial.print("Function = "); Serial.println(function);
    // Serial.print("Value = "); Serial.println(value);
    // Initialise a command to be sent
    Command command {
      comm::handlerTable[static_cast<uint8_t>(function)],
      value
    };

    if(command.function != nullptr){
      command.function(value);
      // xQueueSend(queue, &command, pdMS_TO_TICKS(WAIT_TIME));
    }

  }
}
