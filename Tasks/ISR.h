#pragma once

#include "ROB12629.h"

extern ROB12629 leftEncoder, rightEncoder;

void leftISR(){
  leftEncoder.onInterrupt();
}

void rightISR(){
  rightEncoder.onInterrupt();
}