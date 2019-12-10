#include "RunOnceTimer.h"
#include "CarStates.h"

#ifndef CarControl_h
#define CarControl_h

class CarControl : public EventListener{

 private:
    CarMovement *carMovement;
    EventBus *eventBus;
    CarState *currentState;

  public:
    CarControl(CarMovement *carMovement, EventBus *eventBus);
    void receiveEvent(EventBase *event);
    CarMovement* getCarMovement();
    EventBus* CarControlgetEventBus();
    RunOnceTimer* getRunOnceTimer();
    void setup(){}
};

CarControl::CarControl(CarMovement *carMovement, EventBus *eventBus){
  this->carMovement = carMovement;
  this->eventBus = eventBus;
  this->currentState = new StoppedState(new CarStateControl(carMovement, eventBus));
  this->eventBus->addEventListener(this);
};

void CarControl::receiveEvent(EventBase *event){
   Serial.print(">>>>>> Car control: ");
   Serial.println(event->eventType());
   Serial.print("OLD State: ");
   Serial.println(currentState->stateName());
   CarState *oldState = this->currentState;
   CarEvent *carEvent = (CarEvent *) event;
   this->currentState = this->currentState->transition(carEvent);
   Serial.print("NEW State: ");
   Serial.println(currentState->stateName());
   if (oldState != this->currentState){
     delete oldState;
   }
};

#endif
