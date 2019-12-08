#include "RunOnceTimer.h"
#include "CarStates.h"

#ifndef CarControl_h
#define CarControl_h

class CarControl : public EventListener{

 private:
    CarMovement *carMovement;
    EventBus *eventBus;
    CarState *currentState;
    RunOnceTimer *runOnceTimer = new RunOnceTimer();

  public:
    CarControl(CarMovement *carMovement, EventBus *eventBus);
    void receiveEvent(EventBase *event);
    void setup(){
      runOnceTimer->start(1000);
    }
};

CarControl::CarControl(CarMovement *carMovement, EventBus *eventBus){
  this->carMovement = carMovement;
  this->eventBus = eventBus;
  this->currentState = new StoppedState(carMovement, runOnceTimer);
  this->eventBus->addEventListener(this);
};

void CarControl::receiveEvent(EventBase *event){
  Serial.print("Car control: ");
  Serial.println(event->eventType());
  Serial.print("Current: ");
  Serial.println(currentState->currentState());
  CarState *oldState = this->currentState;
  CarEvent *carEvent = (CarEvent *) event;
  this->currentState = this->currentState->transition(carEvent);
  Serial.print("New: ");
  Serial.println(currentState->currentState());
  if (oldState != this->currentState){
    delete oldState;
  }
};

#endif
