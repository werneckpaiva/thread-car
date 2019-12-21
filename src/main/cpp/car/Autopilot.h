#include "EventBus.h"
#include "CarEvents.h"

#ifndef AutoPilog_h
#define AutoPilog_h

class MonitoringAutoPilotState : public CarState{
  private:
    EventBus *eventBus;
    static const byte MIN_MOVING_DISTANCE = 10;
    static const byte MAX_MOVING_DISTANCE = 30;
    static const float MOVING_DISTANCE_DIFF = (MAX_MOVING_DISTANCE - MIN_MOVING_DISTANCE);
    byte currentSpeed = 0;
  
  public: 
    MonitoringAutoPilotState(EventBus *eventBus);
    CarState* transition(CarEvent *event);
    void processDistance(DistanceDetectedEvent *event);
    void speedChanged(SpeedChangedEvent *event);
    char* stateName(){return "MonitoringAutoPilotState"; };
};

MonitoringAutoPilotState :: MonitoringAutoPilotState(EventBus *eventBus){
  this->eventBus = eventBus;
};

CarState* MonitoringAutoPilotState::transition(CarEvent *event) {
  CarEvent *carEvent = (CarEvent *) event;
  switch(event->eventType()){
    case CarEvent::DISTANCE_DETECTED:
      this->processDistance(carEvent);
      break;
    case CarEvent::SPEED_CHANGED:
      this->speedChanged(carEvent);
      break;
  }
  return this;
};

void MonitoringAutoPilotState :: processDistance(DistanceDetectedEvent *event){
  byte hitDistance = 0;
  if (event->getAngle() >= 70 && event->getAngle() <= 110){
    float proportionalVel = (float) this->currentSpeed / (float) CarMovement::MAX_SPEED;
    hitDistance = MIN_MOVING_DISTANCE + (MOVING_DISTANCE_DIFF * proportionalVel);
  } else {
    // Discard smaller value
    hitDistance = MIN_MOVING_DISTANCE;
  }
  if (event->getDistance() < hitDistance){
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
};

void MonitoringAutoPilotState :: speedChanged(SpeedChangedEvent *event){
  this->currentSpeed = event->getSpeed();
};

#endif
