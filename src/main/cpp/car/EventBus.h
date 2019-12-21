#include "RunOnceTimer.h"

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

    ListenerNode::ListenerNode(EventListener *listener);
    EventListener* ListenerNode::getListener();
};

ListenerNode::ListenerNode(EventListener *listener){
  this->listener = listener;
}

EventListener* ListenerNode::getListener(){
  return this->listener;
}

class EventBus : public RunnableTask{
  private:
    ListenerNode *rootListenerList = NULL;
    RunOnceTimer *runOnceTimer;
    EventBase *nextEvent = NULL;

  public:
    EventBus(){
      this->runOnceTimer =  new RunOnceTimer();
      this->runOnceTimer->start(1000);
    }
  
    void addEventListener(EventListener *listener){
      ListenerNode *current = this->rootListenerList;
      this->rootListenerList = new ListenerNode(listener);
      this->rootListenerList->next = current;
    }

    void dispatchEvent(EventBase *event){
      if (event == NULL) return;
      ListenerNode *currentListenerNode = this->rootListenerList;
      while (currentListenerNode != NULL){
        currentListenerNode->getListener()->receiveEvent(event);
        currentListenerNode = currentListenerNode->next;
      }
      delete(event);
    }

    void dispatchTimedEvent(EventBase *event, int interval, EventBase *nextEvent){
      this->nextEvent = nextEvent;
      this->runOnceTimer->scheduleTask(this, interval);
      this->dispatchEvent(event);
    }

    void execute(){
      if (nextEvent != NULL){
        Serial.println("Timed event!");
        this->dispatchEvent(this->nextEvent);
      }
    }

};

#endif
