#include "Buggy.h"

ROB12629 leftEncoder(2), rightEncoder(3);

void setup() {
  Serial.begin(115200);

  leftEncoder.begin(leftISR);
  rightEncoder.begin(rightISR);

  comm::initialiseHandlerTable();

  initialiseSemaphores();
  
  xTaskCreate(telemetry, "Telemetry", 512, nullptr, 1, &telemetryTaskHandle);
  xTaskCreate(decide, "Make decision to move", 256, nullptr, 2, nullptr);
  xTaskCreate(irSensing, "Sense from IRs", 256, nullptr, 3, nullptr);

  vTaskStartScheduler();
}

void loop() {

}