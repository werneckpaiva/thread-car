#include "CarEvents.h"

#define VERBOSE 1

#ifndef CarModuleControl_h
#define CarModuleControl_h

class CarState {
  public:
    virtual char* stateName();
    virtual CarState* transition(CarEvent *event);
};

class CarModuleControl : public EventListener{

 private:
    CarState *currentState;

  public:
    CarModuleControl(CarState *initialState);
    void receiveEvent(EventBase *event);
    void setup(){}
};

CarModuleControl::CarModuleControl(CarState *initialState){
  this->currentState = initialState;
  EventBus::addEventListener(this);
};

void CarModuleControl::receiveEvent(EventBase *event){
  CarState *oldState = this->currentState;
  CarEvent *carEvent = (CarEvent *) event;
  this->currentState = this->currentState->transition(carEvent);
  if (oldState != this->currentState){
    #if VERBOSE > 0
      Serial.print("State changed: ");
      Serial.print(oldState->stateName());
      Serial.print(" -> ");
      Serial.println(this->currentState->stateName());
    #endif
    delete(oldState);
  }
};

#endif
