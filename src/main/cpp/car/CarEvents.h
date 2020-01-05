#ifndef CarEvents_h
#define CarEvents_h

class CarEvent : public EventBase {
  public:
    enum EventType {
      MOVE_STOP,                // 0
      MOVE_FORWARD,             // 1
      MOVE_BACKWARD,            // 2
      MOVE_LEFT,                // 3
      MOVE_RIGHT,               // 4
      SPIN_RIGHT_OVER,          // 5
      SPIN_LEFT_OVER,           // 6
      TURN_RIGHT_OVER,          // 7
      TURN_LEFT_OVER,           // 8
      SPEED_CHANGED,            // 9
      MOVEMENT_STOPPED,         // 10 
      DETECT_DISTANCE_SCAN,     // 11
      DETECT_DISTANCE_FIXED,    // 12
      DETECT_FULL_SCAN,         // 13
      DISTANCE_DETECTED,        // 14
      FULL_DISTANCE_DETECTED,   // 15
      STOP_DISTANCE_DETECTION   // 16
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
