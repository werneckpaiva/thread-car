#include "EventBus.h"
#include "CarEvents.h"

#define VERBOSE 2

#ifndef AutoPilot_h
#define AutoPilot_h

class MonitoringAutoPilotState : public CarState{
  private:
    static const byte MIN_MOVING_DISTANCE = 10;
    static const byte MAX_MOVING_DISTANCE = 30;
    static constexpr float MOVING_DISTANCE_DIFF = (MAX_MOVING_DISTANCE - MIN_MOVING_DISTANCE);
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

class DrivingAutoPilotState : public CarState {
  public:
    DrivingAutoPilotState();
    CarState* transition(CarEvent *event);
    CarState* processDistance(FullDistanceDetectedEvent *event);
    char* stateName(){return "DrivingAutoPilotState"; };
};

// State machine ------------------------------

CarState* MonitoringAutoPilotState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::DISTANCE_DETECTED:
      return this->processDistance((DistanceDetectedEvent *) event);
    case CarEvent::SPEED_CHANGED:
      this->speedChanged((SpeedChangedEvent *) event);
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
      return this->processDistance((DistanceDetectedEvent *) event);
  }
  return this;
};

CarState* DrivingAutoPilotState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::FULL_DISTANCE_DETECTED:
      return this->processDistance((FullDistanceDetectedEvent *) event);
  }
  return this;
};

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
    return new DrivingAutoPilotState();
  }
  if (counter > 2){
    #if VERBOSE > 0
      Serial.println("Hit twice");
    #endif
    return new DrivingAutoPilotState();
  }
  return this;
};

// DrivingAutoPilotState --------------------------

DrivingAutoPilotState :: DrivingAutoPilotState(){
  EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  EventBus::dispatchEvent(new CarEvent(CarEvent::DETECT_FULL_SCAN));
};

CarState* DrivingAutoPilotState :: processDistance(FullDistanceDetectedEvent *event){
  byte middle = FullDistanceDetectedEvent::NUM_POINTS / 2;
  unsigned int leftDistance = 0;
  unsigned int rightDistance = 0;
  for (byte i=0; i< middle; i++){
    leftDistance += event->distances[i].distance;
//    #if VERBOSE > 0
//      Serial.print("Angle: ");
//      Serial.print(event->distances[i].angle);
//      Serial.print(" Distance: ");
//      Serial.println(event->distances[i].distance);
//    #endif    
  }
  for (byte i=middle + 1; i< FullDistanceDetectedEvent::NUM_POINTS; i++){
    rightDistance += event->distances[i].distance;
//    #if VERBOSE > 0
//      Serial.print("Angle: ");
//      Serial.print(event->distances[i].angle);
//      Serial.print(" Distance: ");
//      Serial.println(event->distances[i].distance);
//    #endif    
  }
//  Serial.print("Distance left: ");
//  Serial.println(leftDistance);
//  Serial.print("Distance right: ");
//  Serial.print(rightDistance);

  EventBus::dispatchEvent(new CarEvent(CarEvent::STOP_DISTANCE_DETECTION));
  EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_BACKWARD), 50);
  EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_STOP), 700);
  
  if (leftDistance > rightDistance){
      EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_RIGHT), 800);
  } else {
     EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_LEFT), 800); 
  }
  EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_STOP), 1250);
  EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_FORWARD), 1300);
  return new MonitoringAutoPilotState();
}

#endif
