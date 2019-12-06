#include <VariableTimedAction.h>

#ifndef CarContrpol_h
#define CarContrpol_h


enum CAR_EVENT {
  STOP,
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT
};

class RunnableTask {
  public:
    virtual void execute(); 
};

class RunOnceTimer : public VariableTimedAction {

  private:
    RunnableTask *currentTask = NULL;
    unsigned long run() {
      if (this->currentTask != NULL){
        currentTask->execute();
      }
      // It will leak!
      this->stop();
      return 0;
    };

  public:
    void scheduleTask(RunnableTask *task, int interval){
      this->currentTask = task;
      this->start(interval);
    };
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


//    enum STATE {
//        MOVING_FORWARD,
//        MOVING_FORWARD_RIGHT,
//        MOVING_FORWARD_LEFT,
//        MOVING_BACKWARD_LEFT,
//        MOVING_BACKWARD_RIGHT,
//        TURNING_RIGHT,
//        TURNING_LEFT
//    };


class CarControl {

 private:
    CarMovement *carMovement;
    CarState *currentState;
    RunOnceTimer *runOnceTimer = new RunOnceTimer();

  public:
    CarControl(CarMovement *carMovement);
    void transition(CAR_EVENT event);
    void setup(){}
};

CarControl::CarControl(CarMovement *carMovement){
  this->carMovement = carMovement;
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
