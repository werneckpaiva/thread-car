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

class EventBusTimedEventRunner;
class EventBus;

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

  public:
    void addEventListener(EventListener *listener);
    void dispatchEvent(EventBase *event);
    EventBusTimedEventRunner* dispatchTimedEvent(EventBase *event, int interval);
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

EventBusTimedEventRunner* EventBus :: dispatchTimedEvent(EventBase *event, int interval){
  EventBusTimedEventRunner *task = new EventBusTimedEventRunner(this, event);
  TaskScheduler::scheduleRunOnceTask(task, interval);
  return task;
};


EventBusTimedEventRunner :: EventBusTimedEventRunner(EventBus *eventBus, EventBase *event){
  this->eventBus = eventBus;
  this->event = event;
};

void EventBusTimedEventRunner :: execute(){
//  Serial.println("Timed event!");
  this->eventBus->dispatchEvent(this->event);
};


#endif
