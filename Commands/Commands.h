#pragma once

namespace comm
{
  constexpr char DELIMITER = ':'; // Commands will look like 'comm:payload'
  constexpr char END = '\n';

  constexpr char STOP = 'E';
  constexpr char START = 'F';

  constexpr char EVENT = 'G';

  constexpr char CHANGE_TARGET = 'K';

  constexpr char CURRENT_SPEED = 'N';
  constexpr char SAVE_EEPROM = 'P';

  constexpr char KP = 'S';
  constexpr char KI = 'T';
  constexpr char KD = 'U';

  static void (*handlerTable[256])(char command, void* payload) = {nullptr};
}

