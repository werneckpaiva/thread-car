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
  Serial.begin(9600);
  TaskScheduler::scheduleRecurrentTask(this, 50);
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
//  Serial.print("SERIAL CMD: <");
//  Serial.print(cmd);
//  Serial.println(">");
  if (cmd=='U') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (cmd=='D') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_BACKWARD));
//    EventBus::dispatchTimedEvent(new CarEvent(CarEvent::MOVE_STOP), 2000);
  } else if (cmd=='L') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (cmd=='R') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (cmd=='S') {
    EventBus::dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
