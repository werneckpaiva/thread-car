#include "EventBus.h"
#include "CarEvents.h"

#define VERBOSE 1

#ifndef AutoPilot_h
#define AutoPilot_h

class MonitoringAutoPilotState : public CarState{
  private:
    static const byte MIN_MOVING_DISTANCE = 10;
    static const byte MAX_MOVING_DISTANCE = 30;
    static const float MOVING_DISTANCE_DIFF = (MAX_MOVING_DISTANCE - MIN_MOVING_DISTANCE);
    byte currentSpeed = 0;
  
  public: 
    CarState* transition(CarEvent *event);
    CarState* processDistance(DistanceDetectedEvent *event);
    void speedChanged(SpeedChangedEvent *event);
    char* stateName(){return "MonitoringAutoPilotState"; };
};

class HitAutoPilotState : public CarState{
  private:
    byte counter = 0;
    byte initialDistance;

  public:
    HitAutoPilotState(DistanceDetectedEvent *event);
    CarState* transition(CarEvent *event);
    CarState* processDistance(DistanceDetectedEvent *event);
    char* stateName(){return "HitAutoPilotState"; };
};

// State machine ------------------------------

CarState* MonitoringAutoPilotState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::DISTANCE_DETECTED:
      return this->processDistance(event);
    case CarEvent::SPEED_CHANGED:
      this->speedChanged(event);
      break;
    case CarEvent::MOVEMENT_STOPPED:
      this->currentSpeed = 0;
      break;
  }
  return this;
};

CarState* HitAutoPilotState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::DISTANCE_DETECTED:
      return this->processDistance(event);
      break;
  }
  return this;
}

// MonitoringAutoPilotState --------------------------

CarState* MonitoringAutoPilotState :: processDistance(DistanceDetectedEvent *event){
  byte hitDistance = 0;
  if (event->getAngle() >= 70 && event->getAngle() <= 110){
    float proportionalVel = (float) this->currentSpeed / (float) CarMovement::MAX_SPEED;
    hitDistance = MIN_MOVING_DISTANCE + (MOVING_DISTANCE_DIFF * proportionalVel);
  } else {
    // Discard smaller value
    hitDistance = MIN_MOVING_DISTANCE;
  }
  if (event->getDistance() < hitDistance){
    return new HitAutoPilotState(event);
  }
  return this;
};

void MonitoringAutoPilotState :: speedChanged(SpeedChangedEvent *event){
  this->currentSpeed = event->getSpeed();
};

// HitAutoPilotState --------------------------------

HitAutoPilotState :: HitAutoPilotState(DistanceDetectedEvent *event){
  #if VERBOSE > 1
    Serial.print("Distance hit: ");
    Serial.print(event->getDistance());
    Serial.println(" cm");
  #endif
  this->initialDistance = event->getDistance();
  EventBus::dispatchEvent(new DetectDistanceFixedEvent(event->getAngle()));
};

CarState* HitAutoPilotState :: processDistance(DistanceDetectedEvent *event){
  #if VERBOSE > 1
    Serial.print("Distance double check: ");
    Serial.print(event->getDistance());
    Serial.println(" cm");
  #endif
  counter++;
  if (event->getDistance() <= this->initialDistance){
     #if VERBOSE > 0
      Serial.print("Hit at ");
      Serial.print(event->getDistance());
      Serial.println(" cm");
    #endif
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
    return new MonitoringAutoPilotState();
  }
  if (counter > 2){
    EventBus::dispatchEvent(new CarEvent(CarEvent::DETECT_DISTANCE_SCAN));
    return new MonitoringAutoPilotState();
  }
  return this;
};


#endif
