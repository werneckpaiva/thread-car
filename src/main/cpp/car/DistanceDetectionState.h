#include "DistanceDetector.h"
#include "CarEvents.h"

#define VERBOSE 2

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

class FullScanDistanceDetectionState : public DistanceDetectionState, public RunnableTask {
  protected:
    static const int HEAD_TIMER = 50;
    static const byte ANGLE_INCREMENT = 10;
    static const byte NUM_POINTS = ((DistanceDetector::MAX_HEAD_ANGLE - DistanceDetector::MIN_HEAD_ANGLE) / ANGLE_INCREMENT) + 1;
    byte currentPoint = 0;
    byte angle = 0;
    bool movesClockwise = true;
    DistanceAndAngle *distances = NULL;

  public:
    FullScanDistanceDetectionState(DistanceDetectionStateControl *control);
    DistanceDetectionState* transition(CarEvent *event);
    char * stateName(){return "FullScanDistanceDetectionState"; };
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

FullScanDistanceDetectionState :: FullScanDistanceDetectionState(DistanceDetectionStateControl *control) : DistanceDetectionState(control) {
  this->angle = 0;
  this->distances = NULL;
  TaskScheduler::scheduleRecurrentTask(this, FullScanDistanceDetectionState::HEAD_TIMER);
};

// State Machine -----------------------------------
DistanceDetectionState* StoppedDistanceDetectionState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_FORWARD:
      return new ScanningDistanceDetectionState(this->control);
    case CarEvent::DETECT_FULL_SCAN:
      return new FullScanDistanceDetectionState(this->control);
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

DistanceDetectionState* FullScanDistanceDetectionState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::FULL_DISTANCE_DETECTED:
      break;
  }
  return this;
};

// Scanning --------------------------------------
void ScanningDistanceDetectionState::execute() {
  byte distance = this->control->getDistanceDetector()->detectDistance();
  byte angle = this->angle;
  #if VERBOSE > 0
    Serial.print("Angle: ");
    Serial.print(this->angle);
    Serial.print(" distance: ");
    Serial.print(distance);
    Serial.println("cm");
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
  #if VERBOSE > 0
    Serial.print("Scan fixed distance: ");
    Serial.print(distance);
    Serial.print("cm angle: ");
    Serial.println(this->angle);
  #endif
  EventBus::dispatchEvent(new DistanceDetectedEvent(this->angle, distance));
};

void FullScanDistanceDetectionState::execute() {
  if (this->distances == NULL){
      this->distances = new DistanceAndAngle[FullScanDistanceDetectionState::NUM_POINTS];
  }
  this->control->getDistanceDetector()->moveHead(this->angle);
  byte distance =  this->control->getDistanceDetector()->detectDistance();
  this->distances[this->currentPoint].distance = distance;
  this->distances[this->currentPoint].angle = this->angle;
  #if VERBOSE > 1
    Serial.print("Scanning currentPoint: ");
    Serial.print(this->currentPoint);
    Serial.print(" angle: ");
    Serial.print(this->angle);
    Serial.print(" distance: ");
    Serial.println(distance);
  #endif
  if (this->movesClockwise){
      this->angle += FullScanDistanceDetectionState::ANGLE_INCREMENT;
      this->currentPoint += 1;
    } else {
      this->angle -= FullScanDistanceDetectionState::ANGLE_INCREMENT;
      this->currentPoint -= 1;
    }
  
  if (this->currentPoint == 255 || this->currentPoint == FullScanDistanceDetectionState::NUM_POINTS){
    if (this->currentPoint == 255) {
      this->currentPoint = 0;
      this->angle = DistanceDetector::MIN_HEAD_ANGLE;
    }
    if (this->currentPoint == FullScanDistanceDetectionState::NUM_POINTS) {
      this->currentPoint = FullScanDistanceDetectionState::NUM_POINTS - 1;
      this->angle = DistanceDetector::MAX_HEAD_ANGLE;
    }
    this->movesClockwise = !this->movesClockwise;
    EventBus::dispatchEvent(new FullDistanceDetectedEvent(this->distances, FullScanDistanceDetectionState::NUM_POINTS));
    this->distances = NULL;
  }
};

#endif
