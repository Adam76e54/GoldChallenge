#include "Buggy.h"

void setup() {
  Serial.begin(115200);

  comm::initialiseHandlerTable();

  initialiseSemaphores();
  
  xTaskCreate(telemetry, "Telemetry", 512, nullptr, 1, &telemetryTaskHandle);
  xTaskCreate(decide, "Make decision to move", 256, nullptr, 3, nullptr);
  xTaskCreate(irSensing, "Sense from IRs", 256, nullptr, 2, nullptr);

  vTaskStartScheduler();
}

void loop() {

}