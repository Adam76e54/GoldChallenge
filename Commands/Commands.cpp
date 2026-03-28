#include <Arduino.h>
#include "Commands.h"
#include "GlobalObjects.h"

namespace comm{

  void (*handlerTable[comm::TABLE_SIZE])(Command& command) = {nullptr};

  void initialiseHandlerTable(){
    handlerTable[static_cast<uint8_t>(comm::STOP_TOGGLE)] = &handleStopToggle;

    for(char c = '0'; c <= '9'; ++c){
      handlerTable[c] = &handleArrays;
    }

  }

  void print(Command&){
    Serial.println("That worked");
  }

  void handleStopToggle(Command& stop){
    if(xSemaphoreTake(stoppedSemaphore, pdMS_TO_TICKS(50))){
      // Read in as a number since the GUI sends 0/1 not true/false
      uint8_t value = atoi(stop.payload);
      // Convert nicely to bool 
      state.stopped = (value != 0);

      Serial.print("Recieved p = "); Serial.println(value);
      // Serial.print("Changed stopped to "); Serial.println(state.stopped ? "true" : "false");

      xSemaphoreGive(stoppedSemaphore);
    }
  }

  void handleArrays(Command& cmd){
    unsigned char name = cmd.name;
    uint8_t value = atoi(cmd.payload);

    for(uint8_t i = 0; i < comm::ARRAY_SIZE; ++i){
      if(name == comm::TIMES[i]){
        data.targetTimes[i] = value;
        break;
      }

      if(name == comm::SPEEDS[i]){
        data.targetSpeeds[i] = value;
        break;
      }
    }
  }


}

