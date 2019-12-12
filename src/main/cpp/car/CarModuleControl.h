#ifndef CarModuleControl_h
#define CarModuleControl_h

class CarState {
  public:
    virtual String stateName();
    virtual CarState* transition(CarEvent *event);
};

class CarModuleControl : public EventListener{

 private:
    EventBus *eventBus;
    CarState *currentState;

  public:
    CarModuleControl(EventBus *eventBus, CarState *initialState);
    void receiveEvent(EventBase *event);
    void setup(){}
};

CarModuleControl::CarModuleControl(EventBus *eventBus, CarState *initialState){
  this->eventBus = eventBus;
  this->currentState = initialState;
  this->eventBus->addEventListener(this);
};

void CarModuleControl::receiveEvent(EventBase *event){
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
