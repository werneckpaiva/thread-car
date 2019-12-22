#include "TaskScheduler.h"

#ifndef EventBus_h
#define EventBus_h

class EventBase{
  public:
    virtual int eventType();
};


class EventListener{
  public: virtual void receiveEvent(EventBase *event);
};


class ListenerNode{
  private:
    EventListener *listener;
  public:
    ListenerNode *next = NULL;

    ListenerNode(EventListener *listener) {
      this->listener = listener;
    }

    EventListener* ListenerNode::getListener(){
      return this->listener;
    }
};


class TimedEventRunnerElement;
class EventBusTimedEventRunner;
class EventBus;

class TimedEventRunnerElement {
  public:
    EventBusTimedEventRunner *runner;
    TimedEventRunnerElement *next;
    TimedEventRunnerElement(EventBusTimedEventRunner *runner, TimedEventRunnerElement *next){
      this->runner = runner;
      this->next = next;
    }
};

class EventBusTimedEventRunner : public RunnableTask {
  private:
    EventBus *eventBus;
    EventBase *event;

  public:
    EventBusTimedEventRunner(EventBus *eventBus, EventBase *event);
    int getEventType(){
      return this->event->eventType();
    }
    void execute();
};

class EventBus{
  private:
    // Keep tracking of listeners
    ListenerNode *rootListenerList = NULL;

    // Keep tracking of pending timed events
    TimedEventRunnerElement *rootTimedEventRunnerList = NULL;

  public:
    void addEventListener(EventListener *listener);
    void dispatchEvent(EventBase *event);
    void dispatchTimedEvent(EventBase *event, int interval);
    void removeEventRunner(EventBusTimedEventRunner *runner);
    void cancelEvent(int eventType);
    byte countPendingEvents(){
        TimedEventRunnerElement *current = this->rootTimedEventRunnerList;
        byte count = 0;
        while(current != NULL){
         count++;
         current = current->next;
        }
        return count;
    }
};

void EventBus :: addEventListener(EventListener *listener){
  ListenerNode *current = this->rootListenerList;
  this->rootListenerList = new ListenerNode(listener);
  this->rootListenerList->next = current;
};

void EventBus :: dispatchEvent(EventBase *event){
  Serial.print("Dispatch EVENT: ");
  Serial.println(event->eventType());

  if (event == NULL) return;
  ListenerNode *currentListenerNode = this->rootListenerList;
  while (currentListenerNode != NULL){
    currentListenerNode->getListener()->receiveEvent(event);
    currentListenerNode = currentListenerNode->next;
  }
  delete(event);
};

void EventBus :: dispatchTimedEvent(EventBase *event, int interval){
  EventBusTimedEventRunner *task = new EventBusTimedEventRunner(this, event);
  TaskScheduler::scheduleRunOnceTask(task, interval);
  this->rootTimedEventRunnerList = new TimedEventRunnerElement(task, this->rootTimedEventRunnerList);
};


EventBusTimedEventRunner :: EventBusTimedEventRunner(EventBus *eventBus, EventBase *event){
  this->eventBus = eventBus;
  this->event = event;
};

void EventBusTimedEventRunner :: execute(){
//  Serial.println("Timed event!");
  this->eventBus->dispatchEvent(this->event);
  this->eventBus->removeEventRunner(this);
};

void EventBus :: removeEventRunner(EventBusTimedEventRunner *runner){
  if (this->rootTimedEventRunnerList == NULL) return;
  if (this->rootTimedEventRunnerList->runner == runner){
    TimedEventRunnerElement *toBeDeallocated = this->rootTimedEventRunnerList;
    this->rootTimedEventRunnerList = this->rootTimedEventRunnerList->next;
    delete(toBeDeallocated);
  } else {
    TimedEventRunnerElement *current = this->rootTimedEventRunnerList;
    while(current->next != NULL){
      if (current->next->runner == runner || !current->next->runner->isTaskRunning()){
        TimedEventRunnerElement *toBeDeallocated = current->next;
        current->next = current->next->next;
        delete(toBeDeallocated);
        break;
      }
      current = current->next;
    }
  }
}

void EventBus :: cancelEvent(int eventType){  
  TimedEventRunnerElement *current = this->rootTimedEventRunnerList;
  while(current != NULL){
    if (current->runner->getEventType() == eventType){
      current->runner->pauseTask();
    }
    current = current->next;
  }
}

#endif
