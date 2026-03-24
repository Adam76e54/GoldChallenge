#pragma once
#include <Arduino.h>

class TCRT5000 {
  uint8_t analog_;

public:
  TCRT5000(uint8_t analog) : analog_(analog) {
    // empty
  }


  unsigned int read() const{
  // Raw analogRead function
    return analogRead(analog_);
  }

};