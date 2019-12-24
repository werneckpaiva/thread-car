
#ifndef TaskScheduler_h
#define TaskScheduler_h

class RunnableTask {

  private:
    bool taskRunning = true;

  public:
    virtual void execute();

    void pauseTask() {
      this->taskRunning = false;
    };

    void startTask() {
      this->taskRunning = true;
    };

    bool isTaskRunning(){
      return this->taskRunning;
    };
};


class TaskQueueElement{

  public:
    unsigned long nextRun = 0;
    RunnableTask* task;
    unsigned int recurrentPeriod = 0;

    TaskQueueElement *nextTaskQueueElement = NULL;
    
    TaskQueueElement(RunnableTask* task, unsigned int period, TaskQueueElement *nextTaskQueueElement = NULL){
      this->task = task;
      this->recurrentPeriod = period;
      this->nextTaskQueueElement = nextTaskQueueElement;
    }

    ~TaskQueueElement(){
      delete(this->task);
    }

    bool isRecurrent(){
      return this->recurrentPeriod > 0;
    }

};

class TaskScheduler{

  private:
    static void removeTaskElementFromQueue(TaskQueueElement *taskQueueElement);
    static TaskQueueElement *rootTaskQueueElement;

  public:
    static void scheduleRecurrentTask(RunnableTask *task, unsigned int period);
    static void scheduleRunOnceTask(RunnableTask *task, unsigned int period);
    static void removeTask(RunnableTask *task);
    static void process();
};

TaskQueueElement *TaskScheduler :: rootTaskQueueElement = NULL;

void TaskScheduler :: process(){
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  TaskQueueElement *nextQueueTask;
  unsigned long currentTime;

  while(currentTaskQueueElement != NULL){
    currentTime = millis();
    nextQueueTask = currentTaskQueueElement->nextTaskQueueElement;
    if (currentTime >= currentTaskQueueElement->nextRun){
      if (currentTaskQueueElement->task->isTaskRunning()){
        currentTaskQueueElement->task->execute(); 
      }
      if (currentTaskQueueElement->isRecurrent()){
        currentTaskQueueElement->nextRun = (currentTime + currentTaskQueueElement->recurrentPeriod);
      } else {
        TaskScheduler::removeTaskElementFromQueue(currentTaskQueueElement);
        delete(currentTaskQueueElement);
      }
    }
    currentTaskQueueElement = nextQueueTask;
  }
};

void TaskScheduler :: removeTaskElementFromQueue(TaskQueueElement *taskQueueElement){
  if (TaskScheduler::rootTaskQueueElement == NULL) return;
  if (TaskScheduler::rootTaskQueueElement == taskQueueElement){
    TaskScheduler::rootTaskQueueElement = TaskScheduler::rootTaskQueueElement->nextTaskQueueElement;
    return;
  }
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  while(currentTaskQueueElement != NULL){
    if (currentTaskQueueElement->nextTaskQueueElement == taskQueueElement){
      currentTaskQueueElement->nextTaskQueueElement = taskQueueElement->nextTaskQueueElement;
      return;
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
};

void TaskScheduler :: removeTask(RunnableTask *task){
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  while(currentTaskQueueElement != NULL){
    if (currentTaskQueueElement->task == task){
      TaskScheduler::removeTaskElementFromQueue(currentTaskQueueElement);
      break;
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
}

void TaskScheduler :: scheduleRecurrentTask(RunnableTask *task, unsigned int period){
  rootTaskQueueElement = new TaskQueueElement(task, period, TaskScheduler::rootTaskQueueElement);
};

void TaskScheduler :: scheduleRunOnceTask(RunnableTask *task, unsigned int period){
  TaskScheduler::rootTaskQueueElement = new TaskQueueElement(task, 0, TaskScheduler::rootTaskQueueElement);
  TaskScheduler::rootTaskQueueElement->nextRun = millis() + period;
};

#endif
