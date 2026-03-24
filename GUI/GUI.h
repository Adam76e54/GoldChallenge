#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include "GlobalObjects.h"
#include "Buffer.h"
#include "Commands.h"
#include "NetworkSetup.h"
#include "Arduino_FreeRTOS.h"

constexpr uint8_t N = 200;
extern Buffer<N> in;
extern WiFiServer server;
extern WiFiClient GUI;

// Keep connection alive
void keep(WiFiClient& GUI, WiFiServer& server){
  if(!GUI){
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

    if(!message) return;

    // Get function opcode as a char
    char function = message[0];
    // Get a value if it exists (we take a pointer to the rest of the string)
    const char* value = (messageSize > 2) ? &message[2] : nullptr;

    // Initialise a command to be sent
    Command command {
      comm::handlerTable[static_cast<uint8_t>(function)],
      value
    };

    if(command.function != nullptr){
      xQueueSend(commandQueue, &command, pdMS_TO_TICKS(WAIT_TIME));  // or a timeout instead of 0
    }

  }
}


void sendEvent(WiFiClient& GUI, const char* event){
  if(GUI && GUI.connected()){
    char message[128];

    int n = snprintf(message, sizeof(message), "%c%c%s", comm::EVENT, comm::DELIMITER, event);

    if(n > 0 && n < (int)sizeof(message)){
      GUI.println(message);
    }

    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    // GUI.println(message);
  }
}

void sendEvent(WiFiClient& GUI, const char* event, float num){
  if(GUI && GUI.connected()){
    char message[128];
    char numberAsString[16];

    dtostrf(num, 8, 2, numberAsString); // apparently %f printf() is disabled by default and this is preferred, no idea why

    int n = snprintf(message, sizeof(message), "%c%c%s%c%s",
      comm::EVENT, comm::DELIMITER, event, comm::DELIMITER, numberAsString);

    if(n > 0 && n < (int)sizeof(message)){
      GUI.println(message);
    }

    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    // GUI.print(message);
    // GUI.println(num);
  }
}