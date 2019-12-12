#include "RunOnceTimer.h"
#include "CarMovement.h"
#include "CarModuleControl.h"

#ifndef CarMovementStates_h
#define CarMovementStates_h

//    enum STATE {
//        MOVING_FORWARD_RIGHT,
//        MOVING_FORWARD_LEFT,
//        MOVING_BACKWARD_LEFT,
//        MOVING_BACKWARD_RIGHT,
//    };

class CarMovementStateControl {
  private:
    RunOnceTimer *runOnceTimer;
    CarMovement *carMovement;
    EventBus *eventBus;
  public:
    CarMovementStateControl(CarMovement *carMovement, EventBus *eventBus){
      this->carMovement = carMovement;
      this->eventBus = eventBus;
      this->runOnceTimer = new RunOnceTimer();
      this->runOnceTimer->start(1000);
    }
    CarMovement* getCarMovement(){ return this->carMovement; };
    EventBus* getEventBus(){ return this->eventBus; };
    RunOnceTimer* getRunOnceTimer(){ return this->runOnceTimer;};
};

class CarMovementState : public CarState{
  protected:
    CarMovementStateControl *control;
  public:
    CarMovementState(CarMovementStateControl *control){
      this->control = control;
    }
};

class StoppedState : public CarMovementState {
  public:
    StoppedState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "Stopped"; };
};

class MovingForwardState : public CarMovementState {

  private:
    static const int ACCELERATION_INCREMENT = 20;
    int initialSpeed;
    CarMovementState* accelerate(int increment);
    CarMovementState* slowDown(int increment);

  public:
//    MovingForwardState(CarMovementStateControl *control);
    MovingForwardState(CarMovementStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingForward"; };
};

class MovingBackwardState : public CarMovementState {

  private:
    static const int ACCELERATION_INCREMENT = 20;
    int initialSpeed;
    CarMovementState* accelerate(int increment);
    CarMovementState* slowDown(int increment);

  public:
    MovingBackwardState(CarMovementStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingBackward"; };
};

class SpinningRightState : public CarMovementState, public RunnableTask{
  private:
    static const int SPINNING_TIMER = 160;
    void spinRight();
  public:
    SpinningRightState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningRight"; };
};

class SpinningLeftState : public CarMovementState, public RunnableTask{

  private:
    static const int SPINNING_TIMER = 160;
    void spinLeft();
  public:
    SpinningLeftState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningLeft"; };
};

class MovingForwardRightState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingForwardRightState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingForwardRight"; };
};

class MovingForwardLeftState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingForwardLeftState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingForwardLeft"; };
};

class MovingBackwardRightState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingBackwardRightState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardRight"; };
};

class MovingBackwardLeftState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingBackwardLeftState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardLeft"; };
};

// Constructors -----------------------------------

StoppedState :: StoppedState(CarMovementStateControl *control) : CarMovementState(control) {
  this->control->getCarMovement()->stop();
};

MovingForwardState :: MovingForwardState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveForward(initialSpeed);
};

MovingBackwardState :: MovingBackwardState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveBackward(initialSpeed);
}

SpinningRightState :: SpinningRightState(CarMovementStateControl *control) : CarMovementState(control) {
  this->spinRight();
};

SpinningLeftState :: SpinningLeftState(CarMovementStateControl *control) : CarMovementState(control) {
  this->spinLeft();
};

MovingForwardRightState :: MovingForwardRightState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingForwardLeftState :: MovingForwardLeftState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

MovingBackwardRightState :: MovingBackwardRightState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingBackwardLeftState :: MovingBackwardLeftState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

// State Machine ---------------------------------------

CarMovementState* StoppedState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_FORWARD:
      return new MovingForwardState(this->control, 20);
    case CarEvent::MOVE_BACKWARD:
      return new MovingBackwardState(this->control);
    case CarEvent::MOVE_RIGHT:
      return new SpinningRightState(this->control);
    case CarEvent::MOVE_LEFT:
      return new SpinningLeftState(this->control);
  }
  return this;
};

CarMovementState* MovingForwardState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_FORWARD:
      return this->accelerate(MovingForwardState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_BACKWARD:
      return this->slowDown(MovingForwardState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_RIGHT:
      return new MovingForwardRightState(this->control, this->initialSpeed);
    case CarEvent::MOVE_LEFT:
      return new MovingForwardLeftState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_FORWARD:
      return this->slowDown(MovingBackwardState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_BACKWARD:
      return this->accelerate(MovingBackwardState::ACCELERATION_INCREMENT);
    case CarEvent::MOVE_RIGHT:
      return new MovingBackwardRightState(this->control, this->initialSpeed);
    case CarEvent::MOVE_LEFT:
      return new MovingBackwardLeftState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* SpinningRightState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_RIGHT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new SpinningRightState(this->control);
    case CarEvent::MOVE_STOP:
    case CarEvent::SPIN_RIGHT_OVER:
      return new StoppedState(this->control);
  }
  return this;
};

CarMovementState* SpinningLeftState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_LEFT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new SpinningLeftState(this->control);
    case CarEvent::MOVE_STOP:
    case CarEvent::SPIN_LEFT_OVER:
      return new StoppedState(this->control);
  }
  return this;
};

CarMovementState* MovingForwardRightState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_RIGHT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingForwardRightState(this->control, this->initialSpeed);
    case CarEvent::TURN_RIGHT_OVER:
      return new MovingForwardState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingForwardLeftState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_LEFT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingForwardLeftState(this->control, this->initialSpeed);
    case CarEvent::TURN_LEFT_OVER:
      return new MovingForwardState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardRightState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_RIGHT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingBackwardRightState(this->control, this->initialSpeed);
    case CarEvent::TURN_RIGHT_OVER:
      return new MovingBackwardState(this->control, this->initialSpeed);
  }
  return this;
};

CarMovementState* MovingBackwardLeftState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_STOP:
      return new StoppedState(this->control);
    case CarEvent::MOVE_LEFT:
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingBackwardLeftState(this->control, this->initialSpeed);
    case CarEvent::TURN_LEFT_OVER:
      return new MovingBackwardState(this->control, this->initialSpeed);
  }
  return this;
};

// Moving Forward -----------------------------------

CarMovementState* MovingForwardState::accelerate(int increment){
  return new MovingForwardState(this->control, this->initialSpeed + increment);
};

CarMovementState* MovingForwardState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) return new StoppedState(this->control);
  return new MovingForwardState(this->control, newSpeed);
};

// Moving Backward -----------------------------------

CarMovementState* MovingBackwardState::accelerate(int increment){
  return new MovingBackwardState(this->control, this->initialSpeed + increment);
};

CarMovementState* MovingBackwardState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) return new StoppedState(this->control);
  return new MovingBackwardState(newSpeed);
};

// Spinning Right ------------------------------------

void SpinningRightState::spinRight(){
  this->control->getCarMovement()->spinRight(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, SpinningRightState::SPINNING_TIMER);
}

void SpinningRightState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::SPIN_RIGHT_OVER));
}

// Spinning Left ------------------------------------

void SpinningLeftState::spinLeft(){
  this->control->getCarMovement()->spinLeft(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, SpinningLeftState::SPINNING_TIMER);
}

void SpinningLeftState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::SPIN_LEFT_OVER));
}

// MovingForwardRight ------------------------------

void MovingForwardRightState::turnRight(){
  this->control->getCarMovement()->turnRightForward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingForwardRightState::TURN_TIMER);
}

void MovingForwardRightState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_RIGHT_OVER));
}

// MovingForwardLeft ------------------------------

void MovingForwardLeftState::turnLeft(){
  this->control->getCarMovement()->turnLeftForward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingForwardLeftState::TURN_TIMER);
}

void MovingForwardLeftState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_LEFT_OVER));
}

// MovingBackwardRight ------------------------------

void MovingBackwardRightState::turnRight(){
  this->control->getCarMovement()->turnRightBackward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingBackwardRightState::TURN_TIMER);
}

void MovingBackwardRightState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_RIGHT_OVER));
}

// MovingForwardLeft ------------------------------

void MovingBackwardLeftState::turnLeft(){
  this->control->getCarMovement()->turnLeftBackward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingBackwardLeftState::TURN_TIMER);
}

void MovingBackwardLeftState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_LEFT_OVER));
}

#endif
