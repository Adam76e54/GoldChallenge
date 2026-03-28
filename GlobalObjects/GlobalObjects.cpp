#include "GlobalObjects.h"
#include <Arduino.h> 
#include <Arduino_FreeRTOS.h>

QueueHandle_t commandQueue;
SemaphoreHandle_t sensorSemaphore, 
                  PIDSemaphore, 
                  arraySemaphore,
                  stoppedSemaphore;
TaskHandle_t moveTaskHandle;

Sensors sensors;
SpeedPercentages speedPercentage;
PIDCoefficients speedMatching;
Arrays data;
DriverState state;