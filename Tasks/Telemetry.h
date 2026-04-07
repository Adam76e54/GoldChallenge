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

void sendBack(WiFiClient GUI, const unsigned char comm, float value);
void keep(WiFiClient& GUI, WiFiServer& server);

template <uint8_t N> 
bool read(WiFiClient& GUI, Buffer<N>& buffer);

template <uint8_t N>
void handle(Buffer<N>& buffer);



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

    // Serial.print("Inside telemetry; Available bytes = "); Serial.println(GUI.available());

    read(GUI, in);

    handle(in);

    // Send back IR sensor data
    static float leftIR = 0, rightIR = 0;
    if(xSemaphoreTake(irSemaphore, pdMS_TO_TICKS(100)) == pdTRUE){

      //  / 1023.0f) * 100.0f
      leftIR = (sensors.left );
      rightIR = (sensors.right);

      xSemaphoreGive(irSemaphore);

      sendBack(GUI, comm::LEFT_IR, leftIR);
      sendBack(GUI, comm::RIGHT_IR, rightIR);
    }

    // Send actual speeds and times
    uint32_t idx;
    float time = 0, speed = 0;

    if(xTaskNotifyWait(0, 0xFFFFFFFF, &idx, pdMS_TO_TICKS(50))){
      access(arraySemaphore, pdMS_TO_TICKS(5), [&idx, &time, &speed](){
        time = (float)data.currentActualTime;
        speed = (float)data.currentActualSpeed;
      });



      char delimiter = static_cast<char>(comm::DELIMITER);
      GUI.print(static_cast<char>(comm::ACTUAL_TIME)); GUI.print(delimiter);
      GUI.print(idx); GUI.print(delimiter) ; GUI.println(time);  

      Serial.print(static_cast<char>(comm::ACTUAL_TIME)); Serial.print(delimiter);
      Serial.print(idx); Serial.print(delimiter) ; Serial.println(time);  
      
      GUI.print(static_cast<char>(comm::ACTUAL_SPEED)); GUI.print(delimiter);
      GUI.print(idx); GUI.print(delimiter) ; GUI.println(speed);
    }



  }
}

// Keep connection alive
void keep(WiFiClient& GUI, WiFiServer& server){
  if (!GUI || !GUI.connected()) {
    if (GUI) GUI.stop();
    GUI = server.available();
  }
}

void sendBack(WiFiClient GUI, const unsigned char comm, float value){
  GUI.print(static_cast<char>(comm)); GUI.print(static_cast<char>(comm::DELIMITER)); GUI.println(value);
  // Serial.print(static_cast<char>(comm)); Serial.print(static_cast<char>(comm::DELIMITER)); Serial.println(value);
  
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

// Handle commands loaded into buffer
template <uint8_t N>
void handle(Buffer<N>& buffer){

  if(buffer.available() > 0){
    // constexpr unsigned int WAIT_TIME = 100;

    char message [N];

    const uint8_t messageSize = buffer.read(message, N);

    Serial.println(message);
    
    // Get function opcode as a char
    unsigned char name = static_cast<unsigned char>(message[0]);
    auto function = comm::handlerTable[name];
    // Get a value if it exists (we take a pointer to the rest of the string)
    const char* value = (messageSize > 2) ? &message[2] : nullptr;

    // Serial.print("Function = "); Serial.println(name);
    // Serial.print("Value = "); Serial.println(value);
    // Initialise a command to be sent
    Command command {
      name,
      value
    };

    if(function != nullptr){
      function(command);
    }

  }
}
