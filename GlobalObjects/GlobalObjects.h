#pragma once

#include <Arduino.h> 
#include <Arduino_FreeRTOS.h>
// - RTOS OBJECTS -
extern QueueHandle_t commandQueue;
extern SemaphoreHandle_t irSemaphore, 
                         speedPIDSemaphore, 
                         arraySemaphore,
                         stoppedSemaphore,
                         thresholdSemaphore,
                         angleSemaphore;
                         
extern TaskHandle_t telemetryTaskHandle;
void initialiseSemaphores();

// Repeatable function for accessing data under the protection of a semaphore
template <typename Function>
bool access(SemaphoreHandle_t semaphore, TickType_t time , Function&& function){
  if(xSemaphoreTake(semaphore, time) == pdTRUE){
    function();
    xSemaphoreGive(semaphore);
    return true;
  }

  return false;
}

// - STRUCTS -
struct Sensors{
  uint16_t left = 0;
  uint16_t right = 0;
};

struct Thresholds{
  uint16_t left = 0;
  uint16_t right = 0;
};

struct SpeedPercentages {
  // Current percentage to feed to driver
  float left = 0;
  float right = 0;
};

struct PIDCoefficients {
  // PID control
  float kp = 0.0;
  float ki = 0.0;
  float kd = 0.0;
};

struct Arrays{
  bool changed = false;

  // Time and speed arrays
  static constexpr uint8_t ARRAY_SIZE = 5;
  uint16_t targetTimes[ARRAY_SIZE] = {0};
  uint16_t targetSpeeds[ARRAY_SIZE] = {0};

  float currentActualTime = 0;
  float currentActualSpeed = 0;
};

// EEPROM addresses
constexpr uint16_t LEFT_PERCENTAGE_ADDRESS = 0;
constexpr uint16_t RIGHT_PERCENTAGE_ADDRESS = LEFT_PERCENTAGE_ADDRESS + sizeof(float);
constexpr uint16_t KP_ADDRESS = RIGHT_PERCENTAGE_ADDRESS + sizeof(float);
constexpr uint16_t KI_ADDRESS = KP_ADDRESS + sizeof(float);
constexpr uint16_t KD_ADDRESS = KI_ADDRESS + sizeof(float);

struct DriverState{
  bool stopped = true;
} ;



extern Sensors sensors;
extern Thresholds thresholds;
extern SpeedPercentages speedPercentage;
extern PIDCoefficients speedCoefficients, angleCoefficients;
extern Arrays data;
extern DriverState state;

