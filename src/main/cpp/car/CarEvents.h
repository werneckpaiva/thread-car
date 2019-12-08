#ifndef CarEvents_h
#define CarEvents_h

class CarEvent : public EventBase {
  public:
    enum EventType {
      MOVE_STOP,
      MOVE_FORWARD,
      MOVE_BACKWARD
    };
};

class CarMoveForwardEvent : public CarEvent {
  public: int eventType(){ return CarEvent::MOVE_FORWARD; }
};

class CarMoveBackwardEvent : public CarEvent {
  public: int eventType(){ return CarEvent::MOVE_BACKWARD; }
};

class CarMoveStopEvent : public CarEvent {
  public: int eventType(){ return CarEvent::MOVE_STOP; }
};

#endif
