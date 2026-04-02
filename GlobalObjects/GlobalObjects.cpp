#include "GlobalObjects.h"
#include <Arduino.h> 
#include <Arduino_FreeRTOS.h>

QueueHandle_t commandQueue;
SemaphoreHandle_t irSemaphore,
                  speedPIDSemaphore, 
                  arraySemaphore,
                  stoppedSemaphore,
                  thresholdSemaphore, 
                  angleSemaphore;
TaskHandle_t telemetryTaskHandle;

Sensors sensors;
Thresholds thresholds;
SpeedPercentages speedPercentage;
PIDCoefficients speedCoefficients, angleCoefficients;
Arrays data;
DriverState state;

void initialiseSemaphores(){
  irSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(irSemaphore);

  speedPIDSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(speedPIDSemaphore);

  arraySemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(arraySemaphore);

  stoppedSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(stoppedSemaphore);

  thresholdSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(thresholdSemaphore);

  angleSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(angleSemaphore);
}
