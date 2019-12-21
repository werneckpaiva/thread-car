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
    void processCommand(char* cmd);
  
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
  size_t len = Serial.available();
  if(len > 0) {
    char cmd[len + 1];
    Serial.readBytesUntil('\n', cmd, len);
    cmd[len] = 0x00;
    this->processCommand(cmd);
    delete(cmd);
  }
};

void SerialCarControl :: processCommand(char *cmd) {
  if (cmd == "UP") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (strcmp(cmd, "DOWN")) {
    this->eventBus->dispatchTimedEvent(new CarEvent(CarEvent::MOVE_BACKWARD), 1000, new CarEvent(CarEvent::MOVE_STOP));
  } else if (strcmp(cmd, "LEFT")) {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (strcmp(cmd, "RIGHT")) {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (strcmp(cmd, "STOP")) {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
