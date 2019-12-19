#include "RunOnceTimer.h"
#include "CarMovement.h"
#include "CarModuleControl.h"

#ifndef CarMovementStates_h
#define CarMovementStates_h

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

class StoppedCarMovementState : public CarMovementState {
  public:
    StoppedCarMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "Stopped"; };
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
    String stateName(){return "MovingForward"; };
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
    String stateName(){return "MovingBackward"; };
};

class SpinningRightStateMovementState : public CarMovementState, public RunnableTask{
  private:
    static const int SPINNING_TIMER = 160;
    void spinRight();
  public:
    SpinningRightStateMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningRight"; };
};

class SpinningLeftMovementState : public CarMovementState, public RunnableTask{

  private:
    static const int SPINNING_TIMER = 160;
    void spinLeft();
  public:
    SpinningLeftMovementState(CarMovementStateControl *control);
    CarMovementState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningLeft"; };
};

class MovingForwardRightMovementState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingForwardRightMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingForwardRight"; };
};

class MovingForwardLeftMovementState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingForwardLeftMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingForwardLeft"; };
};

class MovingBackwardRightMovementState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingBackwardRightMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardRight"; };
};

class MovingBackwardLeftMovementState : public CarMovementState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingBackwardLeftMovementState(CarMovementStateControl *control, int initialSpeed);
    CarMovementState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardLeft"; };
};

// Constructors -----------------------------------

StoppedCarMovementState :: StoppedCarMovementState(CarMovementStateControl *control) : CarMovementState(control) {
  this->control->getCarMovement()->stop();
};

MovingForwardMovementState :: MovingForwardMovementState(CarMovementStateControl *control, int initialSpeed) : CarMovementState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveForward(initialSpeed);
  this->control->getEventBus()->dispatchEvent(new SpeedChangedEvent(initialSpeed));
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
      this->control->getRunOnceTimer()->cancelSchedule();
      return new SpinningRightStateMovementState(this->control);
    case CarEvent::MOVE_STOP:
    case CarEvent::SPIN_RIGHT_OVER:
      return new StoppedCarMovementState(this->control);
  }
  return this;
};

CarMovementState* SpinningLeftMovementState::transition(CarEvent *event) {
  switch(event->eventType()){
    case CarEvent::MOVE_LEFT:
      this->control->getRunOnceTimer()->cancelSchedule();
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
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingForwardRightMovementState(this->control, this->initialSpeed);
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
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingForwardLeftMovementState(this->control, this->initialSpeed);
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
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingBackwardRightMovementState(this->control, this->initialSpeed);
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
      this->control->getRunOnceTimer()->cancelSchedule();
      return new MovingBackwardLeftMovementState(this->control, this->initialSpeed);
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
  if (newSpeed <= 0) return new StoppedCarMovementState(this->control);
  return new MovingForwardMovementState(this->control, newSpeed);
};

// Moving Backward -----------------------------------

CarMovementState* MovingBackwardStateMovementState::accelerate(int increment){
  return new MovingBackwardStateMovementState(this->control, this->initialSpeed + increment);
};

CarMovementState* MovingBackwardStateMovementState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) return new StoppedCarMovementState(this->control);
  return new MovingBackwardStateMovementState(newSpeed);
};

// Spinning Right ------------------------------------

void SpinningRightStateMovementState::spinRight(){
  this->control->getCarMovement()->spinRight(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, SpinningRightStateMovementState::SPINNING_TIMER);
}

void SpinningRightStateMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::SPIN_RIGHT_OVER));
}

// Spinning Left ------------------------------------

void SpinningLeftMovementState::spinLeft(){
  this->control->getCarMovement()->spinLeft(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, SpinningLeftMovementState::SPINNING_TIMER);
}

void SpinningLeftMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::SPIN_LEFT_OVER));
}

// MovingForwardRight ------------------------------

void MovingForwardRightMovementState::turnRight(){
  this->control->getCarMovement()->turnRightForward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingForwardRightMovementState::TURN_TIMER);
}

void MovingForwardRightMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_RIGHT_OVER));
}

// MovingForwardLeft ------------------------------

void MovingForwardLeftMovementState::turnLeft(){
  this->control->getCarMovement()->turnLeftForward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingForwardLeftMovementState::TURN_TIMER);
}

void MovingForwardLeftMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_LEFT_OVER));
}

// MovingBackwardRight ------------------------------

void MovingBackwardRightMovementState::turnRight(){
  this->control->getCarMovement()->turnRightBackward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingBackwardRightMovementState::TURN_TIMER);
}

void MovingBackwardRightMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_RIGHT_OVER));
}

// MovingForwardLeft ------------------------------

void MovingBackwardLeftMovementState::turnLeft(){
  this->control->getCarMovement()->turnLeftBackward(CarMovement::MAX_SPEED);
  this->control->getRunOnceTimer()->scheduleTask(this, MovingBackwardLeftMovementState::TURN_TIMER);
}

void MovingBackwardLeftMovementState::execute(){
  this->control->getEventBus()->dispatchEvent(new CarEvent(CarEvent::TURN_LEFT_OVER));
}

#endif
