#pragma once

#include "ROB12629.h"

// These should probably only be touched in the move() task and ISRs
extern ROB12629 leftEncoder, rightEncoder;

void leftISR(){
  leftEncoder.onInterrupt();
}

void rightISR(){
  rightEncoder.onInterrupt();
}