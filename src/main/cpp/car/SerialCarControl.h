#include "ActionScheduler.h"
#include "EventBus.h"

#ifndef SerialCarControl_h
#define SerialCarControl_h

class SerialCarControl : public ActionScheduler {

  private:
    EventBus *eventBus;

    unsigned long run();

  public:
    SerialCarControl(EventBus *eventBus);
    void setup();
    void processSerialBuffer();
    void processCommand(String cmd);
  
};

SerialCarControl::SerialCarControl(EventBus *eventBus){
  this->eventBus = eventBus;
};

void SerialCarControl :: setup(){
  Serial.begin(9600);
  this->start(20, true);
};

unsigned long SerialCarControl :: run(){
  this->processSerialBuffer();
  return 0;
};

void SerialCarControl :: processSerialBuffer() {
  if(Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    this->processCommand(cmd);
    Serial.flush();
  }
};

void SerialCarControl :: processCommand(String cmd) {
  Serial.print("Serial command: ");
  Serial.println(cmd);
  if (cmd == "UP") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (cmd == "DOWN") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_BACKWARD));
  } else if (cmd == "LEFT") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (cmd == "RIGHT") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (cmd == "STOP") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
