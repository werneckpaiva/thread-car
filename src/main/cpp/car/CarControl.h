#include "RunOnceTimer.h"

#ifndef CarContrpol_h
#define CarContrpol_h


//enum EventType{
//      CAR_MOVEMENT_STOP,
//      CAR_MOVEMENT_FORWARD,
//      CAR_MOVEMENT_BACKWARD,
//      CAR_MOVEMENT_RIGHT,
//      CAR_MOVEMENT_LEFT
//    };


//    enum STATE {
//        MOVING_FORWARD,
//        MOVING_FORWARD_RIGHT,
//        MOVING_FORWARD_LEFT,
//        MOVING_BACKWARD_LEFT,
//        MOVING_BACKWARD_RIGHT,
//        TURNING_RIGHT,
//        TURNING_LEFT
//    };

enum CAR_EVENT {
  STOP,
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT
};



class CarState {
  protected:
    CarMovement *carMovement;
    RunOnceTimer *runOnceTimer;
  public:
    CarState(CarMovement *carMovement, RunOnceTimer *runOnceTimer){
      this->carMovement = carMovement;
      this->runOnceTimer = runOnceTimer;
    }
    virtual CarState* transition(CAR_EVENT event);
};

class StoppedState : public CarState {
  public:
    StoppedState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CAR_EVENT event);
};

class MovingForwardState : public CarState {

  private:
    static const int SPEED_ACCEL = 20;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
    MovingForwardState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CAR_EVENT event);
};

class MovingBackwardState : public CarState {

  private:
    static const int SPEED_ACCEL = 20;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
    MovingBackwardState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CAR_EVENT event);
};

// Stopped -----------------------------------

StoppedState :: StoppedState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->carMovement->stop();
};

CarState* StoppedState::transition(CAR_EVENT event) {
  switch(event){
    case FORWARD:
      return new MovingForwardState(this->carMovement, this->runOnceTimer);
    case BACKWARD:
      return new MovingBackwardState(this->carMovement, this->runOnceTimer);
    case RIGHT:
    case LEFT:
      break;
  }
  return this;
};

// Moving Forward -----------------------------------

MovingForwardState :: MovingForwardState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->accelerate(MovingForwardState::SPEED_ACCEL);
};

CarState* MovingForwardState::accelerate(int increment){
  this->carMovement->moveForward(this->carMovement->getCurrentSpeed() + increment);
  return this;
};

CarState* MovingForwardState::slowDown(int increment){
  if (this->carMovement->getCurrentSpeed() == 0){
    return new MovingBackwardState(this->carMovement, this->runOnceTimer);
  }
  this->carMovement->moveForward(this->carMovement->getCurrentSpeed() - increment);
};

CarState* MovingForwardState::transition(CAR_EVENT event) {
  switch(event){
    case STOP:
      return new StoppedState(this->carMovement, this->runOnceTimer);
    case FORWARD:
      return this->accelerate(MovingForwardState::SPEED_ACCEL);
    case BACKWARD:
      return this->slowDown(MovingForwardState::SPEED_ACCEL);
    case RIGHT:
    case LEFT:
      break;
  }
  return this;
};

// Moving Backward -----------------------------------

MovingBackwardState :: MovingBackwardState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->accelerate(MovingBackwardState::SPEED_ACCEL);
};

CarState* MovingBackwardState::accelerate(int increment){
  this->carMovement->moveBackward(this->carMovement->getCurrentSpeed() + increment);
  return this;
};

CarState* MovingBackwardState::slowDown(int increment){
  if (this->carMovement->getCurrentSpeed() == 0){
    return new MovingForwardState(this->carMovement, this->runOnceTimer);
  }
  this->carMovement->moveBackward(this->carMovement->getCurrentSpeed() - increment);
};

CarState* MovingBackwardState::transition(CAR_EVENT event) {
  switch(event){
    case STOP:
      return new StoppedState(this->carMovement, this->runOnceTimer);
    case FORWARD:
      return this->accelerate(MovingBackwardState::SPEED_ACCEL);
    case BACKWARD:
      return this->slowDown(MovingBackwardState::SPEED_ACCEL);
  }
  return this;
};

class CarControl {

 private:
    CarMovement *carMovement;
    EventBus *eventBus;
    CarState *currentState;
    RunOnceTimer *runOnceTimer = new RunOnceTimer();

  public:
    CarControl(CarMovement *carMovement, EventBus *eventBus);
    void transition(CAR_EVENT event);
    void setup(){
      runOnceTimer->start(1000);
    }
};

CarControl::CarControl(CarMovement *carMovement, EventBus *eventBus){
  this->carMovement = carMovement;
  this->eventBus = eventBus;
  this->currentState = new StoppedState(carMovement, runOnceTimer);
};

void CarControl::transition(CAR_EVENT event){
  CarState *oldState = this->currentState;
  this->currentState = this->currentState->transition(event);
  if (oldState != this->currentState){
    delete oldState;
  }
};

#endif
