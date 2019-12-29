#include "TaskScheduler.h"

#define VERBOSE 0

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
  public:
    EventBase *event;
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
    static void cancelTimedEvent(EventBusTimedEventRunner *task);
};

ListenerNode * EventBus :: rootListenerList = NULL;


void EventBus :: addEventListener(EventListener *listener){
  ListenerNode *current = EventBus::rootListenerList;
  EventBus::rootListenerList = new ListenerNode(listener);
  EventBus::rootListenerList->next = current;
};

void EventBus :: dispatchEvent(EventBase *event){
  #if VERBOSE > 0
    Serial.print("Dispatch EVENT: ");
    Serial.println(event->eventType());
  #endif
  if (event == NULL) return;
  ListenerNode *currentListenerNode = EventBus::rootListenerList;
  while (currentListenerNode != NULL){
    currentListenerNode->getListener()->receiveEvent(event);
    currentListenerNode = currentListenerNode->next;
  }
  #if VERBOSE > 1
    Serial.println("Deleting event");
  #endif;
  delete(event);
};

void EventBus :: cancelTimedEvent(EventBusTimedEventRunner *task){
  Serial.println("Cancel timed event");
  delete(task->event);
  TaskScheduler::removeTask(task);
  delete(task);
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
  #if VERBOSE > 0
    Serial.println("Timed event!");
  #endif
  EventBus::dispatchEvent(this->event);
  this->event = NULL;
};


#endif
