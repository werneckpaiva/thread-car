#define VERBOSE 0

#include "EventBus.h"

#ifndef SerialCarControl_h
#define SerialCarControl_h

class SerialCarControl : public RunnableTask {

  private:
    TaskScheduler *scheduler;
    void execute();

  public:
    void setup();
    void processSerialBuffer();
    void processCommand(char cmd);
  
};

void SerialCarControl :: setup(){
  TaskScheduler::scheduleRecurrentTask(this, 30);
};

void SerialCarControl :: execute(){
  this->processSerialBuffer();
};

void SerialCarControl :: processSerialBuffer() {
  size_t len = Serial.available();
  if(len > 0) {
//    String cmd = Serial.readStringUntil('\n');
    char cmd = Serial.read();
    this->processCommand(cmd);
  }
};

void SerialCarControl :: processCommand(char cmd) {
  #if VERBOSE > 0
    Serial.print("SERIAL CMD: <");
    Serial.print(cmd);
    Serial.println(">");
  #endif
  if (cmd=='U') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (cmd=='D') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_BACKWARD));
  } else if (cmd=='L') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (cmd=='R') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (cmd=='S') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
