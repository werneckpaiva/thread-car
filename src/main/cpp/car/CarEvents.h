#ifndef CarEvents_h
#define CarEvents_h

class CarEvent : public EventBase {
  public:
    enum EventType {
      MOVE_STOP,
      MOVE_FORWARD,
      MOVE_BACKWARD,
      MOVE_LEFT,
      MOVE_RIGHT,
      SPIN_RIGHT_OVER,
      SPIN_LEFT_OVER,
      TURN_RIGHT_OVER,
      TURN_LEFT_OVER,
      SPEED_CHANGED,
      MOVEMENT_STOPPED,
      DETECT_DISTANCE_SCAN,
      DETECT_DISTANCE_FIXED,
      DETECT_FULL_SCAN,
      DISTANCE_DETECTED,
      FULL_DISTANCE_DETECTED
    };
    CarEvent(EventType event);
    int eventType();

  protected:
    virtual ~CarEvent() = default;

  private:
    EventType event;
};

CarEvent::CarEvent(EventType event) {
  this->event = event;
};

int CarEvent::eventType() {
  return this->event;
};

class DistanceDetectedEvent : public CarEvent{
  private:
    byte angle;
    byte distance;

  public:
    DistanceDetectedEvent(byte angle, byte distance) : CarEvent (CarEvent::DISTANCE_DETECTED){
      this->angle = angle;
      this->distance = distance;
    };

    byte getAngle(){ return this->angle; };
    byte getDistance(){ return this->distance; };

};

class SpeedChangedEvent : public CarEvent{
  private:
    byte newSpeed;

  public:
    SpeedChangedEvent(byte newSpeed) : CarEvent (CarEvent::SPEED_CHANGED){
      this->newSpeed = newSpeed;
    };

    byte getSpeed(){ return this->newSpeed; };

};

class DetectDistanceFixedEvent : public CarEvent {
  private:
    byte angle;

  public:
    DetectDistanceFixedEvent(byte angle) : CarEvent(CarEvent::DETECT_DISTANCE_FIXED){
      this->angle = angle;
    }
    byte getAngle(){ return this->angle; };
};

struct DistanceAndAngle {
  byte distance;
  byte angle;
};

class FullDistanceDetectedEvent : public CarEvent {
  public:
    static const byte NUM_POINTS = 19;
    DistanceAndAngle distances[NUM_POINTS];

    FullDistanceDetectedEvent() : CarEvent(CarEvent::FULL_DISTANCE_DETECTED){}
};

#endif
