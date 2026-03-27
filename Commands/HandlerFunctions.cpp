#include "HandlerFunctions.h"
#include "Commands.h"

void print(void*){
  Serial.println("That worked");
}

void handleStopToggle(void* stop){
  state.stopped = *(static_cast<bool>(stop));
}