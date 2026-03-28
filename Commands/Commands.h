#pragma once

#include <Arduino.h>
#include "GlobalObjects.h"

namespace comm
{
  constexpr unsigned char DELIMITER = ':'; // Commands will look like 'comm:payload'
  constexpr unsigned char END = '\n';

  constexpr unsigned char LEFT_IR = 'A';
  constexpr unsigned char RIGHT_IR = 'B';

  constexpr unsigned char comm_RIGHT_IR_OUT = 'F';
  constexpr unsigned char comm_LEFT_IR_OUT = 'J';

  constexpr unsigned char KP = 'C';
  constexpr unsigned char KI = 'D';
  constexpr unsigned char KD = 'E';

  constexpr unsigned char STOP_TOGGLE = 'G';

  constexpr unsigned char CURRENT_SPEED = 'H';
  constexpr unsigned char SAVE_EEPROM = 'I';

  constexpr unsigned char TIMES[] = {'1', '2', '3', '4', '5'};
  constexpr unsigned char SPEEDS[] = {'0', '6', '7', '8', '9'};

  constexpr uint16_t TABLE_SIZE = 256;
  extern void (*handlerTable[TABLE_SIZE])(const char* payload);
  void initialiseHandlerTable();

  void print(const char*);

  void handleStopToggle(const char* stop);

}

struct Command {
  void (*function)(const char*);
  const char* payload;
};
