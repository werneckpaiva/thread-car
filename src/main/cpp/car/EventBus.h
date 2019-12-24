#include "TaskScheduler.h"

#define VERBOSE 1

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
    EventBase *event;

  public:
    EventBusTimedEventRunner(EventBase *event);
    int getEventType(){
      return this->event->eventType();
    }
    void execute();
};

class EventBus{
  private:
    // Keep tracking of listeners
    static ListenerNode *rootListenerList;

  public:
    static void addEventListener(EventListener *listener);
    static void dispatchEvent(EventBase *event);
    static EventBusTimedEventRunner* dispatchTimedEvent(EventBase *event, int interval);
};

ListenerNode * EventBus :: rootListenerList = NULL;


void EventBus :: addEventListener(EventListener *listener){
  ListenerNode *current = EventBus::rootListenerList;
  EventBus::rootListenerList = new ListenerNode(listener);
  EventBus::rootListenerList->next = current;
};

void EventBus :: dispatchEvent(EventBase *event){
  #ifdef VERBOSE
    Serial.print("Dispatch EVENT: ");
    Serial.println(event->eventType());
  #endif
  if (event == NULL) return;
  ListenerNode *currentListenerNode = EventBus::rootListenerList;
  while (currentListenerNode != NULL){
    currentListenerNode->getListener()->receiveEvent(event);
    currentListenerNode = currentListenerNode->next;
  }
  delete(event);
};

EventBusTimedEventRunner* EventBus :: dispatchTimedEvent(EventBase *event, int interval){
  EventBusTimedEventRunner *task = new EventBusTimedEventRunner(event);
  TaskScheduler::scheduleRunOnceTask(task, interval);
  return task;
};


EventBusTimedEventRunner :: EventBusTimedEventRunner(EventBase *event){
  this->event = event;
};

void EventBusTimedEventRunner :: execute(){
  #ifdef VERBOSE
    Serial.println("Timed event!");
  #endif
  EventBus::dispatchEvent(this->event);
};


#endif
