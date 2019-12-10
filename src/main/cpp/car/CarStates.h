#ifndef CarContrpol_h
#define CarContrpol_h

//    enum STATE {
//        MOVING_FORWARD_RIGHT,
//        MOVING_FORWARD_LEFT,
//        MOVING_BACKWARD_LEFT,
//        MOVING_BACKWARD_RIGHT,
//    };

class CarStateControl {
  private:
    RunOnceTimer *runOnceTimer;
    CarMovement *carMovement;
    EventBus *eventBus;
  public:
    CarStateControl(CarMovement *carMovement, EventBus *eventBus){
      this->carMovement = carMovement;
      this->eventBus = eventBus;
      this->runOnceTimer = new RunOnceTimer();
      this->runOnceTimer->start(1000);
    }
    CarMovement* getCarMovement(){ return this->carMovement; };
    EventBus* getEventBus(){ return this->eventBus; };
    RunOnceTimer* getRunOnceTimer(){ return this->runOnceTimer;};
};

class CarState {
  protected:
    CarStateControl *control;
  public:
    virtual String stateName();
    CarState(CarStateControl *control){
      this->control = control;
    }
    virtual CarState* transition(CarEvent *event);
};

class StoppedState : public CarState {
  public:
    StoppedState(CarStateControl *control);
    CarState* transition(CarEvent *event);
    String stateName(){return "Stopped"; };
};

class MovingForwardState : public CarState {

  private:
    static const int ACCELERATION_INCREMENT = 20;
    int initialSpeed;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
//    MovingForwardState(CarStateControl *control);
    MovingForwardState(CarStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingForward"; };
};

class MovingBackwardState : public CarState {

  private:
    static const int ACCELERATION_INCREMENT = 20;
    int initialSpeed;
    CarState* accelerate(int increment);
    CarState* slowDown(int increment);

  public:
    MovingBackwardState(CarStateControl *control, int initialSpeed=ACCELERATION_INCREMENT);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingBackward"; };
};

class SpinningRightState : public CarState, public RunnableTask{
  private:
    static const int SPINNING_TIMER = 160;
    void spinRight();
  public:
    SpinningRightState(CarStateControl *control);
    CarState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningRight"; };
};

class SpinningLeftState : public CarState, public RunnableTask{

  private:
    static const int SPINNING_TIMER = 160;
    void spinLeft();
  public:
    SpinningLeftState(CarStateControl *control);
    CarState* transition(CarEvent *event);
    void execute();
    String stateName(){return "SpinningLeft"; };
};

class MovingForwardRightState : public CarState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingForwardRightState(CarStateControl *control, int initialSpeed);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingForwardRight"; };
};

class MovingForwardLeftState : public CarState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingForwardLeftState(CarStateControl *control, int initialSpeed);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingForwardLeft"; };
};

class MovingBackwardRightState : public CarState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnRight();
    void execute();
  public:
    MovingBackwardRightState(CarStateControl *control, int initialSpeed);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardRight"; };
};

class MovingBackwardLeftState : public CarState, public RunnableTask{

  private:
    static const int TURN_TIMER = 160;
    int initialSpeed=0;
    void turnLeft();
    void execute();
  public:
    MovingBackwardLeftState(CarStateControl *control, int initialSpeed);
    CarState* transition(CarEvent *event);
    String stateName(){return "MovingBackwardLeft"; };
};

// Constructors -----------------------------------

StoppedState :: StoppedState(CarStateControl *control) : CarState(control) {
  this->control->getCarMovement()->stop();
};

MovingForwardState :: MovingForwardState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveForward(initialSpeed);
};

MovingBackwardState :: MovingBackwardState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->control->getCarMovement()->moveBackward(initialSpeed);
}

SpinningRightState :: SpinningRightState(CarStateControl *control) : CarState(control) {
  this->spinRight();
};

SpinningLeftState :: SpinningLeftState(CarStateControl *control) : CarState(control) {
  this->spinLeft();
};

MovingForwardRightState :: MovingForwardRightState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingForwardLeftState :: MovingForwardLeftState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

MovingBackwardRightState :: MovingBackwardRightState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->turnRight();
};

MovingBackwardLeftState :: MovingBackwardLeftState(CarStateControl *control, int initialSpeed) : CarState(control) {
  this->initialSpeed = initialSpeed;
  this->turnLeft();
};

// State Machine ---------------------------------------

CarState* StoppedState::transition(CarEvent *event) {
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

CarState* MovingForwardState::transition(CarEvent *event) {
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

CarState* MovingBackwardState::transition(CarEvent *event) {
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

CarState* SpinningRightState::transition(CarEvent *event) {
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

CarState* SpinningLeftState::transition(CarEvent *event) {
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

CarState* MovingForwardRightState::transition(CarEvent *event) {
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

CarState* MovingForwardLeftState::transition(CarEvent *event) {
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

CarState* MovingBackwardRightState::transition(CarEvent *event) {
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

CarState* MovingBackwardLeftState::transition(CarEvent *event) {
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

CarState* MovingForwardState::accelerate(int increment){
  return new MovingForwardState(this->control, this->initialSpeed + increment);
};

CarState* MovingForwardState::slowDown(int increment){
  int newSpeed = this->initialSpeed - increment;
  if (newSpeed <= 0) return new StoppedState(this->control);
  return new MovingForwardState(this->control, newSpeed);
};

// Moving Backward -----------------------------------

CarState* MovingBackwardState::accelerate(int increment){
  return new MovingBackwardState(this->control, this->initialSpeed + increment);
};

CarState* MovingBackwardState::slowDown(int increment){
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
