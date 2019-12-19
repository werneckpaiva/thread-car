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

class EventBus{
  private:
    ListenerNode *rootListenerList = NULL;

  public:
    void addEventListener(EventListener *listener){
      ListenerNode *current = this->rootListenerList;
      this->rootListenerList = new ListenerNode(listener);
      this->rootListenerList->next = current;
    }

    void dispatchEvent(EventBase *event){
      if (event == NULL) return;
      ListenerNode *currentListenerNode = this->rootListenerList;
      int count=0;
      while (currentListenerNode != NULL){
        count++;
        currentListenerNode->getListener()->receiveEvent(event);
        currentListenerNode = currentListenerNode->next;
      }
      delete(event);
    }
};

#endif
