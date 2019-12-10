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
      TURN_LEFT_OVER
    };
    CarEvent(EventType event);
    int CarEvent::eventType();

  private:
    EventType event;
};

CarEvent::CarEvent(EventType event) {
  this->event = event;
};

int CarEvent::eventType() {
  return this->event;
};

#endif
