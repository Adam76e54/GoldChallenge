#include <Arduino.h>
#include "Commands.h"
#include "GlobalObjects.h"
#include <Arduino_FreeRTOS.h>

namespace comm{

  void (*handlerTable[comm::TABLE_SIZE])(Command& command) = {nullptr};

  void initialiseHandlerTable(){
    handlerTable[static_cast<uint8_t>(comm::STOP_TOGGLE)] = &handleStopToggle;

    for(unsigned char c = '0'; c <= '9'; ++c){
      handlerTable[static_cast<uint8_t>(c)] = &handleArrays;
    }

    handlerTable[static_cast<uint8_t>(comm::KP)] = &handleCoefficients;
    handlerTable[static_cast<uint8_t>(comm::KI)] = &handleCoefficients;
    handlerTable[static_cast<uint8_t>(comm::KD)] = &handleCoefficients;

  }

  void print(Command&){
    // Serial.println("That worked");
  }

  void handleStopToggle(Command& stop){
    if(xSemaphoreTake(stoppedSemaphore, pdMS_TO_TICKS(50)) == pdTRUE){
      // Read in as a number since the GUI sends 0/1 not true/false
      uint8_t value = atoi(stop.payload);
      // Convert nicely to bool 
      state.stopped = (value != 0);

      // Serial.print("Recieved p = "); Serial.println(value);
      // Serial.print("Changed stopped to "); Serial.println(state.stopped ? "true" : "false");

      xSemaphoreGive(stoppedSemaphore);
    }
  }

  void handleArrays(Command& cmd){
    unsigned char name = cmd.name;
    uint8_t value = atoi(cmd.payload);

    if(xSemaphoreTake(arraySemaphore, pdMS_TO_TICKS(50)) == pdTRUE){

      // Linear search for a matching name
      for(uint8_t i = 0; i < comm::ARRAY_SIZE; ++i){
        if(name == comm::TIMES[i]){
          data.targetTimes[i] = value;
          data.changed = true;
          break;
        }

        if(name == comm::SPEEDS[i]){
          data.targetSpeeds[i] = value;
          data.changed = true;
          break;
        }
      }

      xSemaphoreGive(arraySemaphore);
    }

  }

  void handleThresholds(Command& cmd){
    if(xSemaphoreTake(thresholdSemaphore, pdMS_TO_TICKS(50)) == pdTRUE){

      constexpr uint16_t MAX = 1023;
      switch (cmd.name) {
        case comm::LEFT_IR:
          thresholds.left = atof(cmd.payload);
        break;
        
        case comm::RIGHT_IR:
          thresholds.right = atof(cmd.payload);
        break;
      }

      xSemaphoreGive(thresholdSemaphore);
    }

  }

  void handleTurningFactor(Command& cmd){
    if(xSemaphoreTake(angleSemaphore, pdMS_TO_TICKS(5)) == pdTRUE){

      angleCoefficients.kp = atof(cmd.payload);
      
      xSemaphoreGive(angleSemaphore);
    }
  }

  void handleCoefficients(Command& cmd){
    unsigned char name = cmd.name;
    float value = atof(cmd.payload);

    // Serial.println("Entered handling coefficients");

    access(speedPIDSemaphore, pdMS_TO_TICKS(5), [name, value](){
      switch(name){
        case comm::KP:
          speedCoefficients.kp = value;
          // Serial.print("Adjusted kp to = "); Serial.println(speedCoefficients.kp);
        break;

        case comm::KI:
          speedCoefficients.ki = value;
        break;

        case comm::KD:
          speedCoefficients.kd = value;
        break;
      }
    });
  }
}

