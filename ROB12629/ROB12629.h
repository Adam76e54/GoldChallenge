#pragma once

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

class ROB12629{
  private:
    const uint8_t pin_;//one digital pin
    volatile unsigned long count_;
    volatile unsigned long lastISRTime_;

  public:

    static constexpr float COUNTS_PER_REV_ = 8.0f; 
    static constexpr unsigned int DEBOUNCE_ = 15000;

    ROB12629(uint8_t pin) : pin_(pin), count_(0), lastISRTime_(0) {
      //empty
    }

    //requires us to feed an ISR to it
    void begin(void (*ISR)()){
      pinMode(pin_, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pin_), ISR, CHANGE);
    }
    //NOTE: the reason you can' have an ISR method is because C++ always implicitly passes *this. E.g. Class::func() calls will always be fed Class::func(this)
    //so it autogenerates a parameter. An ISR must have no parameters so needs to be a "free" function (not attached to an instance of a class)

    void onInterrupt() {
    // Called only from the ISR:
      auto now = micros();
      if(now - lastISRTime_ >= DEBOUNCE_){
      count_++;

        lastISRTime_ = now;
      }
    }

    unsigned long count() const{
      taskENTER_CRITICAL();
      auto count = count_;
      taskEXIT_CRITICAL();
      return count;
    }

    unsigned long lastEdgeTime() const {
        taskENTER_CRITICAL();
        auto t = lastISRTime_;
        taskEXIT_CRITICAL();
        return t;
    }

    void reset(){
      taskENTER_CRITICAL();
      count_ = 0;
      taskEXIT_CRITICAL();
    }

    uint8_t pin() const{
      return pin_;
    }

  private:

};
