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
    void processCommand(String cmd);
  
};

SerialCarControl::SerialCarControl(EventBus *eventBus){
  this->eventBus = eventBus;
};

void SerialCarControl :: setup(){
  Serial.begin(9600);
  TaskScheduler::scheduleRecurrentTask(this, 20);
};

void SerialCarControl :: execute(){
  this->processSerialBuffer();
};

void SerialCarControl :: processSerialBuffer() {
  size_t len = Serial.available();
  if(len > 0) {
    String cmd = Serial.readStringUntil('\n');
    this->processCommand(cmd);
  }
};

void SerialCarControl :: processCommand(String cmd) {
  Serial.print("SERIAL CMD: <");
  Serial.print(cmd);
  Serial.println(">");
  if (cmd=="UP") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_FORWARD));
  } else if (cmd=="DOWN") {
    this->eventBus->dispatchTimedEvent(new CarEvent(CarEvent::MOVE_BACKWARD), 1000, new CarEvent(CarEvent::MOVE_STOP));
  } else if (cmd=="LEFT") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_LEFT));
  } else if (cmd=="RIGHT") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_RIGHT));
  } else if (cmd=="STOP") {
    this->eventBus->dispatchEvent(new CarEvent(CarEvent::MOVE_STOP));
  }
}

#endif
