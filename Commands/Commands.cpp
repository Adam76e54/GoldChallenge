#include "Commands.h"
#include "HandlerFunctions.h"

namespace comm{

  void (*handlerTable[TABLE_SIZE])(void* payload) = {nullptr};

  void initialiseHandlerTable(){

  }
}