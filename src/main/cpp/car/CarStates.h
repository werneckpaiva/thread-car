#ifndef CarContrpol_h
#define CarContrpol_h

//    enum STATE {
//        MOVING_FORWARD,
//        MOVING_FORWARD_RIGHT,
//        MOVING_FORWARD_LEFT,
//        MOVING_BACKWARD_LEFT,
//        MOVING_BACKWARD_RIGHT,
//        TURNING_RIGHT,
//        TURNING_LEFT
//    };

class CarState {
  protected:
    CarMovement *carMovement;
    RunOnceTimer *runOnceTimer;
  public:
    CarState(CarMovement *carMovement, RunOnceTimer *runOnceTimer){
      this->carMovement = carMovement;
      this->runOnceTimer = runOnceTimer;
    }
    virtual CarState* transition(CarEvent *event);
    virtual String currentState();
};

class StoppedState : public CarState {
  public:
    StoppedState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CarEvent *event);
    String currentState(){
      return "STOPPED";
    }
};

class MovingForwardState : public CarState {

  private:
    static const int SPEED_ACCEL = 20;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
    MovingForwardState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CarEvent *event);
    String currentState(){
      return "FORWARDING";
    }
};

class MovingBackwardState : public CarState {

  private:
    static const int SPEED_ACCEL = 20;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
    MovingBackwardState(CarMovement *carMovement, RunOnceTimer *timer);
    CarState* transition(CarEvent *event);
    String currentState(){
      return "BACKWARDING";
    }
};

// Constructors -----------------------------------

StoppedState :: StoppedState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->carMovement->stop();
};

MovingForwardState :: MovingForwardState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->accelerate(MovingForwardState::SPEED_ACCEL);
};

MovingBackwardState :: MovingBackwardState(CarMovement *carMovement, RunOnceTimer *timer) : CarState(carMovement, timer) {
  this->accelerate(MovingBackwardState::SPEED_ACCEL);
};

// State Machine ---------------------------------------

CarState* StoppedState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_FORWARD:
      return new MovingForwardState(this->carMovement, this->runOnceTimer);
    case CarEvent::MOVE_BACKWARD:
      return new MovingBackwardState(this->carMovement, this->runOnceTimer);
      break;
  }
  return this;
};

CarState* MovingForwardState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->carMovement, this->runOnceTimer);
    case CarEvent::MOVE_FORWARD:
      return this->accelerate(MovingForwardState::SPEED_ACCEL);
    case CarEvent::MOVE_BACKWARD:
      return this->slowDown(MovingForwardState::SPEED_ACCEL);
      break;
  }
  return this;
};

CarState* MovingBackwardState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->carMovement, this->runOnceTimer);
    case CarEvent::MOVE_FORWARD:
      return this->accelerate(MovingBackwardState::SPEED_ACCEL);
    case CarEvent::MOVE_BACKWARD:
      return this->slowDown(MovingBackwardState::SPEED_ACCEL);
  }
  return this;
};

// Moving Forward -----------------------------------

CarState* MovingForwardState::accelerate(int increment){
  this->carMovement->moveForward(this->carMovement->getCurrentSpeed() + increment);
  return this;
};

CarState* MovingForwardState::slowDown(int increment){
  if (this->carMovement->getCurrentSpeed() <= CarMovement::MIN_MOVING_SPEED){
    return new StoppedState(this->carMovement, this->runOnceTimer);
  }
  this->carMovement->moveForward(this->carMovement->getCurrentSpeed() - increment);
  return this;
};

// Moving Backward -----------------------------------

CarState* MovingBackwardState::accelerate(int increment){
  this->carMovement->moveBackward(this->carMovement->getCurrentSpeed() + increment);
  return this;
};

CarState* MovingBackwardState::slowDown(int increment){
  if (this->carMovement->getCurrentSpeed() <= CarMovement::MIN_MOVING_SPEED){
    return new MovingForwardState(this->carMovement, this->runOnceTimer);
  }
  this->carMovement->moveBackward(this->carMovement->getCurrentSpeed() + increment);
  return this;
};

#endif
