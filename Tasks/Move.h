#pragma once

#include <Arduino.h>
#include "L293D.h"
#include "HCSR04.h"
#include "ROB12629.h"

void move(void*){
  L293D driver(6, 7, 11, 12, 9, 10);

  HCSR04 ears(8, A5);

  ROB12629 left(2), right(3);

  
}