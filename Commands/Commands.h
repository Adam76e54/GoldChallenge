#pragma once

namespace comm
{
  constexpr char DELIMITER = ':'; // Commands will look like 'comm:payload'
  constexpr char END = '\n';

  constexpr char LEFT_IR = 'A';
  constexpr char RIGHT_IR = 'B';

  constexpr char KP = 'C';
  constexpr char KI = 'D';
  constexpr char KD = 'E';

  constexpr char START = 'F';
  constexpr char STOP = 'G';

  constexpr char CURRENT_SPEED = 'H';
  constexpr char SAVE_EEPROM = 'I';

  constexpr char TIMES = {'1', '2', '3', '4', '5'};
  constexpr char SPEEDS = {'0', '6', '7', '8', '9'};

  constexpr uint16_t TABLE_SIZE = 256;
  extern void (*handlerTable[TABLE_SIZE])(void* payload);
}

struct Command {
  void (*function)(void*);
  const void* payload;
};
