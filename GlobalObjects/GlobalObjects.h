#pragma once

#include <Arduino.h> 
#include <Arduino_FreeRTOS.h>
// - RTOS OBJECTS -
extern QueueHandle_t commandQueue;
extern SemaphoreHandle_t sensorSemaphore, 
                         PIDSemaphore, 
                         arraySemaphore,
                         stoppedSemaphore;
// - STRUCS -
struct Sensors{
  uint16_t left = 0;
  uint16_t right = 0;
} sensors;

struct SpeedPercentages {
  // Current percentage to feed to driver
  float left = 0;
  float right = 0;
} speedPercentages;

struct PIDCoefficients {
  // PID control
  float kp = 0.0;
  float ki = 0.0;
  float kd = 0.0;
} straightening, matching;

struct Arrays{
  // Time and speed arrays
  static constexpr uint8_t ARRAY_SIZE = 5;
  unsigned int targetTimes[ARRAY_SIZE];
  unsigned int targetSpeeds[ARRAY_SIZE];

  unsigned int actualTimes[ARRAY_SIZE];
  unsigned int actualSpeeds[ARRAY_SIZE];
} data;

// EEPROM addresses
constexpr unsigned int LEFT_PERCENTAGE_ADDRESS = 0;
constexpr unsigned int RIGHT_PERCENTAGE_ADDRESS = LEFT_PERCENTAGE_ADDRESS + sizeof(float);
constexpr unsigned int KP_ADDRESS = RIGHT_PERCENTAGE_ADDRESS + sizeof(float);
constexpr unsigned int KI_ADDRESS = KP_ADDRESS + sizeof(float);
constexpr unsigned int KD_ADDRESS = KI_ADDRESS + sizeof(float);

struct DriverState{
  bool stopped = true;
} state;