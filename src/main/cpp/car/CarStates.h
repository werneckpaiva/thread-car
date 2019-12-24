#include "CarMovement.h"
#include "CarModuleControl.h"

#define VERBOSE 1

#ifndef CarMovementStates_h
#define CarMovementStates_h

class CarMovementStateControl {
  private:
    CarMovement *carMovement;
  public:
    CarMovementStateControl(CarMovement *carMovement){
      this->carMovement = carMovement;
    }
    CarMovement* getCarMovement(){ return this->carMovement; };
};

class CarMovementState : public CarState{
  protected:
    CarMovementStateControl *control;
  public:
    CarMovementState(CarMovementStateControl *control){
      this->control = control;
    }
};

class StoppedCarMovementState : public CarMovementState {
  public:
    StoppedCarMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "Stopped"; };
};

class MovingForwardMovementState : public CarMovementState {
  private:
    int initialSpeed;
    CarMovementState* accelerate(int increment);
    CarMovementState* slowDown(int increment);

  public:
    static const int ACCELERATION_INCREMENT = 20;
    MovingForwardMovementState(CarMovementStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingForward"; };
};

class MovingBackwardStateMovementState : public CarMovementState {
  private:
    int initialSpeed;
    CarMovementState* accelerate(int increment);
    CarMovementState* slowDown(int increment);

  public:
    static const int ACCELERATION_INCREMENT = 20;
    MovingBackwardStateMovementState(CarMovementStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingBackward"; };
};

class SpinningRightStateMovementState : public CarMovementState{
  private:
    static const int SPINNING_TIMER = 160;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void spinRight();
  public:
    SpinningRightStateMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "SpinningRight"; };
};

class SpinningLeftMovementState : public CarMovementState{
  private:
    static const int SPINNING_TIMER = 160;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void spinLeft();
  public:
    SpinningLeftMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "SpinningLeft"; };
};

class MovingForwardRightMovementState : public CarMovementState{
  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void turnRight();
  public:
    MovingForwardRightMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingForwardRight"; };
};

class MovingForwardLeftMovementState : public CarMovementState {
  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void turnLeft();
  public:
    MovingForwardLeftMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingForwardLeft"; };
};

class MovingBackwardRightMovementState : public CarMovementState{
  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void turnRight();
  public:
    MovingBackwardRightMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingBackwardRight"; };
};

class MovingBackwardLeftMovementState : public CarMovementState{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    EventBusTimedEventRunner* timedEventRunner = NULL;
    void turnLeft();
  public:
    MovingBackwardLeftMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    char *stateName(){return "MovingBackwardLeft"; };
};

// Constructors -----------------------------------

StoppedCarMovementState :: StoppedCarMovementState(CarMovementStateControl *control) : CarMovementState(control) {
  this->control->getCarMovement()->stop();
};

MovingForwardMovementState :: MovingForwardMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveForward(initialSpeed);
  EventBus::dispatchEvent(new SpeedChangedEvent(initialSpeed));
};

MovingBackwardStateMovementState :: MovingBackwardStateMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveBackward(initialSpeed);
}

SpinningRightStateMovementState :: SpinningRightStateMovementState(CarMovementStateControl *control) : CarMovementState(control) {
  this->spinRight();
};

SpinningLeftMovementState :: SpinningLeftMovementState(CarMovementStateControl *control) : CarMovementState(control) {
  this->spinLeft();
};

MovingForwardRightMovementState :: MovingForwardRightMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingForwardLeftMovementState :: MovingForwardLeftMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

MovingBackwardRightMovementState :: MovingBackwardRightMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingBackwardLeftMovementState :: MovingBackwardLeftMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

// State Machine ---------------------------------------

CarMovementState* StoppedCarMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_FORWARD:
      return new MovingForwardMovementState(this->control, MovingForwardMovementState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_BACKWARD:
      return new MovingBackwardStateMovementState(this->control);
    case CarEvent::MOVE_RIGHT:
      return new SpinningRightStateMovementState(this->control);
    case CarEvent::MOVE_LEFT:
      return new SpinningLeftMovementState(this->control);
  }
  return this;
};

CarMovementState* MovingForwardMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_FORWARD:
      return this->accelerate(MovingForwardMovementState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_BACKWARD:
      return this->slowDown(MovingForwardMovementState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_RIGHT:
      return new MovingForwardRightMovementState(this->control, this->initialSpeed);
    case CarEvent::MOVE_LEFT:
      return new MovingForwardLeftMovementState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardStateMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_FORWARD:
      return this->slowDown(MovingBackwardStateMovementState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_BACKWARD:
      return this->accelerate(MovingBackwardStateMovementState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_RIGHT:
      return new MovingBackwardRightMovementState(this->control, this->initialSpeed);
    case CarEvent::MOVE_LEFT:
      return new MovingBackwardLeftMovementState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* SpinningRightStateMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_RIGHT:
      this->spinRight();
      break;
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::SPIN_RIGHT_OVER:
      return new StoppedCarMovementState(this->control);
  }
  return this;
};

CarMovementState* SpinningLeftMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_LEFT:
        this->spinLeft();
        break;
      return new SpinningLeftMovementState(this->control);
    case CarEvent::MOVE_STOP:
    case CarEvent::SPIN_LEFT_OVER:
      return new StoppedCarMovementState(this->control);
  }
  return this;
};

CarMovementState* MovingForwardRightMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_RIGHT:
      this->turnRight();
      break;
    case CarEvent::TURN_RIGHT_OVER:
      return new MovingForwardMovementState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingForwardLeftMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_LEFT:
      this->turnLeft();
      break;
    case CarEvent::TURN_LEFT_OVER:
      return new MovingForwardMovementState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardRightMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_RIGHT:
      this->turnRight();
      break;
    case CarEvent::TURN_RIGHT_OVER:
      return new MovingBackwardStateMovementState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardLeftMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedCarMovementState(this->control);
    case CarEvent::MOVE_LEFT:
      this->turnLeft();
      break;
    case CarEvent::TURN_LEFT_OVER:
      return new MovingBackwardStateMovementState(this->control, this->initialSpeed);
  }
  return this;
};

// Moving Forward -----------------------------------

CarMovementState* MovingForwardMovementState::accelerate(int increment){
  return new MovingForwardMovementState(this->control, this->initialSpeed + increment);
};

CarMovementState* MovingForwardMovementState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
    return new StoppedCarMovementState(this->control);
  }
  return new MovingForwardMovementState(this->control, newSpeed);
};

// Moving Backward -----------------------------------

CarMovementState* MovingBackwardStateMovementState::accelerate(int increment){
  return new MovingBackwardStateMovementState(this->control, this->initialSpeed + increment);
};

CarMovementState* MovingBackwardStateMovementState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
    return new StoppedCarMovementState(this->control);
  }
  return new MovingBackwardStateMovementState(newSpeed);
};

// Spinning Right ------------------------------------

void SpinningRightStateMovementState::spinRight(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->spinRight(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::SPIN_RIGHT_OVER),
    SpinningRightStateMovementState::SPINNING_TIMER);
}

// Spinning Left ------------------------------------

void SpinningLeftMovementState::spinLeft(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->spinLeft(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::SPIN_LEFT_OVER), SpinningLeftMovementState::SPINNING_TIMER);
}

// MovingForwardRight ------------------------------

void MovingForwardRightMovementState::turnRight(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->turnRightForward(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::TURN_RIGHT_OVER), MovingForwardRightMovementState::TURN_TIMER);
}

// MovingForwardLeft ------------------------------

void MovingForwardLeftMovementState::turnLeft(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->turnLeftForward(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::TURN_LEFT_OVER), MovingForwardLeftMovementState::TURN_TIMER);
}

// MovingBackwardRight ------------------------------

void MovingBackwardRightMovementState::turnRight(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->turnRightBackward(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::TURN_RIGHT_OVER), MovingBackwardRightMovementState::TURN_TIMER);
}

// MovingForwardLeft ------------------------------

void MovingBackwardLeftMovementState::turnLeft(){
  if (this->timedEventRunner != NULL) {
    this->timedEventRunner->pauseTask();
  }
  this->control->getCarMovement()->turnLeftBackward(CarMovement::MAX_SPEED);
  this->timedEventRunner = EventBus::dispatchTimedEvent(
    new CarEvent(CarEvent::TURN_LEFT_OVER), MovingBackwardLeftMovementState::TURN_TIMER);
}

#endif
