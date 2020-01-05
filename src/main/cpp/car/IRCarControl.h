#include <IRremote.h>
#include <MemoryFree.h>

#define IR_KEY_UP_1          6375
#define IR_KEY_UP_2          -7177
#define IR_KEY_UP_3          -3698

#define IR_KEY_DOWN_1        5499
#define IR_KEY_DOWN_2        19125
#define IR_KEY_DOWN_3        -2901

#define IR_KEY_RIGHT_1       0x00FF5AA5
#define IR_KEY_RIGHT_2       0x0449E79F
#define IR_KEY_LEFT_1        0x00FF10EF
#define IR_KEY_LEFT_2        0x8C22657B
#define IR_KEY_STOP_1        0x00FF38C7
#define IR_KEY_STOP_2        0x488F3CBB

#ifndef IRCarControl_h
#define IRCarControl_h


class IRCarControl : public RunnableTask {

  private:
    IRrecv *irrecv;
    decode_results irResults;

    static const byte MAX_TIME_BETWEEN_KEYS = 150;

    int lastSelectedKey = -1;
    unsigned long lastSelectedTime = 0;

    void execute();

    char* taskName(){ return "IRCarControl"; };

  public:
    IRCarControl(int irPin);
    CarEvent* readIrKeyAsCarEvent();
    void setup(){
      TaskScheduler::scheduleRecurrentTask(this, 10);
      irrecv->enableIRIn();
    }
};

IRCarControl::IRCarControl(int irPin){
  this->irrecv = new IRrecv(irPin);
};

CarEvent* IRCarControl::readIrKeyAsCarEvent(){
  if (irrecv->decode(&this->irResults)){
      int currentValue = this->irResults.value;
      irrecv->resume();
//      Serial.print("FreeMemory: ");
//      Serial.print(freeMemory());
//      Serial.println(" bytes"); 
      unsigned long currentTime = millis();
      if (currentValue == 0XFFFFFFFF || currentValue == 0XFF) {
        if (currentTime < this->lastSelectedTime + MAX_TIME_BETWEEN_KEYS){
          currentValue = this->lastSelectedKey;
        }
      } else {
        this->lastSelectedKey = currentValue;
      }
      this->lastSelectedTime = currentTime;
      CarEvent *carEvent = NULL;
      switch(currentValue){
        case IR_KEY_UP_1:
        case IR_KEY_UP_2:
        case IR_KEY_UP_3:
          carEvent = new CarEvent(CarEvent::MOVE_FORWARD);
          break;
        case IR_KEY_DOWN_1:
        case IR_KEY_DOWN_2:
        case IR_KEY_DOWN_3:
          carEvent = new CarEvent(CarEvent::MOVE_BACKWARD);
          break;
        case IR_KEY_RIGHT_1:
        case IR_KEY_RIGHT_2:
          carEvent = new CarEvent(CarEvent::MOVE_RIGHT);
          break;
        case IR_KEY_LEFT_1:
        case IR_KEY_LEFT_2:
            carEvent = new CarEvent(CarEvent::MOVE_LEFT); 
            break;
        case IR_KEY_STOP_1:
        case IR_KEY_STOP_2:
          carEvent = new CarEvent(CarEvent::MOVE_STOP);
          break;
      }
      return carEvent;
  }
  return NULL;
};

void IRCarControl::execute(){
  CarEvent *event = this->readIrKeyAsCarEvent();
  if (event != NULL) {
    EventBus::dispatchEvent(event);
  }
};

#endif
