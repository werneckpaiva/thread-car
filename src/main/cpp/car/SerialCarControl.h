#include "EventBus.h"

#ifndef SerialCarControl_h
#define SerialCarControl_h

class SerialCarControl : public RunnableTask {

  private:
    EventBus *eventBus;
    TaskScheduler *scheduler;
    void execute();

  public:
    SerialCarControl(EventBus *eventBus);
    void setup();
    void processSerialBuffer();
    void processCommand(char cmd);
  
};

SerialCarControl::SerialCarControl(EventBus *eventBus){
  this->eventBus = eventBus;
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
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (cmd=='D') {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_BACKWARD));
    this->eventBus->dispatchTimedEvent(new CarEvent(CarEvent::MOVE_STOP), 2000);
  } else if (cmd=='L') {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (cmd=='R') {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (cmd=='S') {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
