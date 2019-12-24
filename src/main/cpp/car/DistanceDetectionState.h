#include "DistanceDetector.h"

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

class StoppedDistanceDetectionState : public DistanceDetectionState {
  public:
    StoppedDistanceDetectionState(DistanceDetectionStateControl *control);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "StoppedDistanceDetectionState"; };
};

class ScanningDistanceDetectionState : public DistanceDetectionState, public RunnableTask {
  protected:
    static const byte ANGLE_INCREMENT = 10;
    static const byte HEAD_TIMER = 50;
  
    byte angle = 0;
    boolean movesClockwise = false;
    byte waitingMiddleCounter = 0;

  public:
    ScanningDistanceDetectionState(DistanceDetectionStateControl *control);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "ScanningDetectionState"; };
    void execute();
};

// Constructors -----------------------------------
StoppedDistanceDetectionState :: StoppedDistanceDetectionState(DistanceDetectionStateControl *control) : DistanceDetectionState(control) {
  this->control->getDistanceDetector()->moveHead(90);
};

ScanningDistanceDetectionState :: ScanningDistanceDetectionState(DistanceDetectionStateControl *control) : DistanceDetectionState(control) {
  TaskScheduler::scheduleRecurrentTask(this, ScanningDistanceDetectionState::HEAD_TIMER);
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
    case CarEvent::MOVE_STOP:
      TaskScheduler::removeTask(this);
      return new StoppedDistanceDetectionState(this->control);
  }
  return this;
};


// Scanning --------------------------------------
void ScanningDistanceDetectionState::execute() {
  byte distance = this->control->getDistanceDetector()->detectDistance();
  EventBus::dispatchEvent(new DistanceDetectedEvent(this->angle, distance));
  
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
};

#endif
