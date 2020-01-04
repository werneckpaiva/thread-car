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

    EventListener* getListener(){
      return this->listener;
    }
};

class EventNode{
  public:
    EventBase *event;
    EventNode *next = NULL;

    EventNode(EventBase *event, EventNode *next=NULL){
      this->event = event;
      this->next = next;
    }

    ~EventNode() {
      delete(this->event);
    }

    EventBase *getEvent(){ return this->event; }
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

class EventBusRunner : public RunnableTask {
  public:
    void execute();
};

class EventBus{
  private:
    // Keep tracking of listeners
    static ListenerNode *rootListenerList;
    static EventNode *rootEventList;
    static EventBusRunner *eventBusRunner;

  public:
    static void addEventListener(EventListener *listener);
    static void dispatchEvent(EventBase *event);
    static EventBusTimedEventRunner* dispatchTimedEvent(EventBase *event, int interval);
    static void cancelTimedEvent(EventBusTimedEventRunner *task);
    static void processAllEvents();
    static void setup();
};

ListenerNode * EventBus :: rootListenerList = NULL;
EventNode * EventBus :: rootEventList = NULL;
EventBusRunner * EventBus :: eventBusRunner = NULL;


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
  EventNode *eventNode = new EventNode(event);
  if (EventBus::rootEventList == NULL){
    EventBus::rootEventList = eventNode;
    return;
  }
  EventNode *currentNode = EventBus::rootEventList;
  while(currentNode->next != NULL){
    currentNode = currentNode->next;
  }
  currentNode->next = eventNode;
};

void EventBus :: processAllEvents(){
  if (EventBus::rootEventList == NULL) return;
  ListenerNode *currentListenerNode;
  EventNode *currentEventNode = EventBus::rootEventList;
  EventNode *eventNodeToDelete = NULL;
  while(currentEventNode != NULL){
    EventBase *event = currentEventNode->event;
    #if VERBOSE > 0
      Serial.print("Processing EVENT: ");
      Serial.println(event->eventType());
    #endif
    currentListenerNode = EventBus::rootListenerList;
    while (currentListenerNode != NULL){
      currentListenerNode->getListener()->receiveEvent(event);
      currentListenerNode = currentListenerNode->next;
    }
    #if VERBOSE > 1
      Serial.print("Deleting event");
      Serial.println(event->eventType());
    #endif;
    eventNodeToDelete = currentEventNode;
    currentEventNode = currentEventNode->next;
    delete(eventNodeToDelete);
  }
  EventBus::rootEventList = NULL;
};

void EventBus :: cancelTimedEvent(EventBusTimedEventRunner *task){
  #if VERBOSE > 0
    Serial.println("Cancel timed event");
  #endif
  delete(task->event);
  TaskScheduler::removeTask(task);
  delete(task);
};

EventBusTimedEventRunner* EventBus :: dispatchTimedEvent(EventBase *event, int interval){
  EventBusTimedEventRunner *task = new EventBusTimedEventRunner(event);
  TaskScheduler::scheduleRunOnceTask(task, interval);
  return task;
};

void EventBus :: setup(){
  EventBus::eventBusRunner = new EventBusRunner();
  TaskScheduler::scheduleRecurrentTask(EventBus::eventBusRunner, 0);
}

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

void EventBusRunner :: execute(){
  EventBus::processAllEvents();
}

#endif
