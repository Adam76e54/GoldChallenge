#include <Arduino.h>
#include "Commands.h"
#include "GlobalObjects.h"

namespace comm{

  void (*handlerTable[comm::TABLE_SIZE])(const char* payload) = {nullptr};

  void initialiseHandlerTable(){
    handlerTable[static_cast<uint8_t>(comm::STOP_TOGGLE)] = &handleStopToggle;
  }

  void print(const char*){
    Serial.println("That worked");
  }

  void handleStopToggle(const char* stop){
    if(xSemaphoreTake(stoppedSemaphore, pdMS_TO_TICKS(50))){
      // Read in as a number since the GUI sends 0/1 not true/false
      uint8_t value = atoi(stop);
      // Convert nicely to bool 
      state.stopped = (value != 0);

      Serial.print("Recieved p = "); Serial.println(value);
      // Serial.print("Changed stopped to "); Serial.println(state.stopped ? "true" : "false");

      xSemaphoreGive(stoppedSemaphore);
    }
  }
}

