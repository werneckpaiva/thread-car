#include "DistanceDetector.h"
#include "CarEvents.h"

#define VERBOSE 0

#ifndef DistanceDetectionState_h
#define DistanceDetectionState_h

class DistanceDetectionStateControl {
  private:
    DistanceDetector *distanceDetector;

  public:
    DistanceDetectionStateControl(DistanceDetector *distanceDetector){
      this->distanceDetector = distanceDetector;
    }
    DistanceDetector* getDistanceDetector(){ return this->distanceDetector; };
};

class DistanceDetectionState : public CarState{
  protected:
    DistanceDetectionStateControl *control;
  public:
    DistanceDetectionState(DistanceDetectionStateControl *control){
      this->control = control;
    }
};

// States ---------------------------

class StoppedDistanceDetectionState : public DistanceDetectionState {
  public:
    StoppedDistanceDetectionState(DistanceDetectionStateControl *control);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "StoppedDistanceDetectionState"; };
};

class ScanningDistanceDetectionState : public DistanceDetectionState, public RunnableTask {
  protected:
    static const byte ANGLE_INCREMENT = 10;
    static const byte HEAD_TIMER = 30;
  
    byte angle = 0;
    boolean movesClockwise = false;
    byte waitingMiddleCounter = 0;

  public:
    ScanningDistanceDetectionState(DistanceDetectionStateControl *control);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "ScanningDetectionState"; };
    void execute();
};

class ScanningSpecificPointState : public DistanceDetectionState, public RunnableTask {
  protected:
    static const byte HEAD_TIMER = 10;
    byte angle;
  public:
    ScanningSpecificPointState(DistanceDetectionStateControl *control, DetectDistanceFixedEvent *event);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "ScanningSpecificPointState"; };
    void execute();
};

// Constructors -----------------------------------
StoppedDistanceDetectionState :: StoppedDistanceDetectionState(DistanceDetectionStateControl *control) : DistanceDetectionState(control) {
  this->control->getDistanceDetector()->moveHead(90);
};

ScanningDistanceDetectionState :: ScanningDistanceDetectionState(DistanceDetectionStateControl *control) : DistanceDetectionState(control) {
  this->angle = this->control->getDistanceDetector()->getHeadAngle();
  
  TaskScheduler::scheduleRecurrentTask(this, ScanningDistanceDetectionState::HEAD_TIMER);
};

ScanningSpecificPointState :: ScanningSpecificPointState(DistanceDetectionStateControl *control, DetectDistanceFixedEvent *event) : DistanceDetectionState(control) {
  TaskScheduler::scheduleRecurrentTask(this, ScanningSpecificPointState::HEAD_TIMER);
  this->angle = event->getAngle();
};

// State Machine -----------------------------------
DistanceDetectionState* StoppedDistanceDetectionState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_FORWARD:
      return new ScanningDistanceDetectionState(this->control);
  }
  return this;
};

DistanceDetectionState* ScanningDistanceDetectionState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVEMENT_STOPPED:
      TaskScheduler::removeTask(this);
      return new StoppedDistanceDetectionState(this->control);
    case CarEvent::DETECT_DISTANCE_FIXED:
      TaskScheduler::removeTask(this);
      return new ScanningSpecificPointState(this->control, event);
  }
  return this;
};

DistanceDetectionState* ScanningSpecificPointState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVEMENT_STOPPED:
      TaskScheduler::removeTask(this);
      return new StoppedDistanceDetectionState(this->control);
    case CarEvent::DETECT_DISTANCE_SCAN:
      TaskScheduler::removeTask(this);
      return new ScanningDistanceDetectionState(this->control);
  }
  return this;
};

// Scanning --------------------------------------
void ScanningDistanceDetectionState::execute() {
  byte distance = this->control->getDistanceDetector()->detectDistance();
  byte angle = this->angle;
  #if VERBOSE > 1
    Serial.print("Scan distance: ");
    Serial.print(distance);
    Serial.print("cm angle: ");
    Serial.println(this->angle);
  #endif
  
  if (this->angle >= DistanceDetector::MAX_HEAD_ANGLE 
    || this->angle <= DistanceDetector::MIN_HEAD_ANGLE){
    this->movesClockwise = !this->movesClockwise;
  }
  if (this->angle == DistanceDetector::MID_HEAD_ANGLE && this->waitingMiddleCounter < 10){
    this->waitingMiddleCounter++;
  } else {
    this->waitingMiddleCounter = 0;
    if (this->movesClockwise){
      this->angle += ScanningDistanceDetectionState::ANGLE_INCREMENT;  
    } else {
      this->angle -= ScanningDistanceDetectionState::ANGLE_INCREMENT;  
    }
  }
  this->control->getDistanceDetector()->moveHead(this->angle);
  EventBus::dispatchEvent(new DistanceDetectedEvent(angle, distance));
};

void ScanningSpecificPointState::execute() {
  this->control->getDistanceDetector()->moveHead(this->angle);
  byte distance = this->control->getDistanceDetector()->detectDistance();
  #if VERBOSE > 1
    Serial.print("Scan fixed distance: ");
    Serial.print(distance);
    Serial.print("cm angle: ");
    Serial.println(this->angle);
  #endif
  EventBus::dispatchEvent(new DistanceDetectedEvent(this->angle, distance));
};

#endif
