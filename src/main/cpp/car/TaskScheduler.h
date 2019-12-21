#include "RunOnceTimer.h"

#ifndef TaskScheduler_H
#define TaskScheduler_H

class TaskStruct{
  RunnableTask* task;
  unsigned long nextRun;
  unsigned int period = 0;
  RunnableTask *nextTask = NULL;
  
}

class TaskScheduler{

  private:
    TaskStruct *queueRoot = NULL;

  public:
    void scheduleRecurrentTask(RunnableTask *task, unsigned int period);
    void scheduleRunOnceTask(RunnableTask *task, unsigned int period);
    void process();

    
}

void TaskScheduler : process(){
  TaskStruct *element = this->queueRoot;
  while(element != null){
    if (millis() >= element->nextRun();
    element = element->nextTask;
  }
}

void TaskScheduler : scheduleRecurrentTask(RunnableTask *task, unsigned int period){
  
}

void TaskScheduler : scheduleRunOnceTask(RunnableTask *task, unsigned int period){
  
}


#endif
