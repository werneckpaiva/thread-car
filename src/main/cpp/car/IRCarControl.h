#include "ActionScheduler.h"

#include <IRremote.h>

#define IR_KEY_UP_1          0x00FF18E7
#define IR_KEY_UP_2          0x3D9AE3F7
#define IR_KEY_DOWN_1        0x00FF4AB5
#define IR_KEY_DOWN_2        0x1BC0157B
#define IR_KEY_RIGHT_1       0x00FF5AA5
#define IR_KEY_RIGHT_2       0x0449E79F
#define IR_KEY_LEFT_1        0x00FF10EF
#define IR_KEY_LEFT_2        0x8C22657B
#define IR_KEY_STOP_1        0x00FF38C7
#define IR_KEY_STOP_2        0x488F3CBB

#ifndef IRCarControl_h
#define IRCarControl_h

class IRCarControl : public ActionScheduler {

  private:
    EventBus *eventBus;

    IRrecv *irrecv;
    decode_results irResults;

    int lastSelectedKey;

    unsigned long run();

  public:
    IRCarControl(int irPin, EventBus *eventBus);
    CarEvent* readIrKeyAsCarEvent();
    void setup(){
      this->start(20, false);
    }
};

IRCarControl::IRCarControl(int irPin, EventBus *eventBus){
  this->eventBus = eventBus;
  this->irrecv = new IRrecv(irPin);
};

CarEvent* IRCarControl::readIrKeyAsCarEvent(){
  if (this->irrecv->decode(&this->irResults)){
      int currentValue = this->irResults.value;
      this->irrecv->resume();

      if (currentValue == 0XFFFFFFFF || currentValue == 0XFF) {
          if (*this->irResults.rawbuf < 1000){
            currentValue = -1;
          } else {
            currentValue = this->lastSelectedKey;
          }
      }

      CarEvent *carEvent = NULL;
      switch(currentValue){
        case IR_KEY_UP_1:
        case IR_KEY_UP_2:
          carEvent = new CarMoveForwardEvent();
          break;
        case IR_KEY_DOWN_1:
        case IR_KEY_DOWN_2:
          carEvent = new CarMoveBackwardEvent();
          break;
//        case IR_KEY_RIGHT_1:
//        case IR_KEY_RIGHT_2:
//          break;
//        case IR_KEY_LEFT_1:
//        case IR_KEY_LEFT_2:
//          break;
        case IR_KEY_STOP_1:
        case IR_KEY_STOP_2:
          carEvent = new CarMoveStopEvent();
          break;
      }
      if (currentValue >= 0){
        this->lastSelectedKey = currentValue;
      }
      return carEvent;
  }
  return NULL;
}


unsigned long IRCarControl::run(){
  CarEvent *eventType = this->readIrKeyAsCarEvent();
  this->eventBus->dispatchEvent(eventType);
  return 0;
};

#endif
