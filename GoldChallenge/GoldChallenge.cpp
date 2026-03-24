#include "Buggy.h"

QueueHandle_t commandQueue;

void setup(){
  commandQueue = xQueueCreate(10, sizeof(Command));
  configASSERT(commandQueue != nullptr);

  xTaskCreate(telemetry, "Telemetry", 1024, nullptr, 1, nullptr);
}

void loop(){
  
}