#include "Buggy.h"


void setup() {
  Serial.begin(115200);

  comm::initialiseHandlerTable();

  commandQueue = xQueueCreate(10, sizeof(Command));
  configASSERT(commandQueue != nullptr);

  stoppedSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(stoppedSemaphore);
  
  xTaskCreate(telemetry, "Telemetry", 1024, nullptr, 1, nullptr);
  xTaskCreate(move, "Move", 1024, nullptr, 2, &moveTaskHandle);
  xTaskCreate(irSensing, "Sense from IRs", 1024, nullptr, 3, nullptr);

  vTaskStartScheduler();
}

void loop() {
}