#define VERBOSE 1

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

    virtual char* taskName();
};


class TaskQueueElement{

  public:
    unsigned long nextRun = 0;
    RunnableTask* task;
    int recurrentPeriod = 0;
    boolean markForDeletion = false;

    TaskQueueElement *nextTaskQueueElement = NULL;
    
    TaskQueueElement(RunnableTask* task, int period, TaskQueueElement *nextTaskQueueElement = NULL){
      this->task = task;
      this->recurrentPeriod = period;
      this->nextTaskQueueElement = nextTaskQueueElement;
    }

    bool isRecurrent(){
      return this->recurrentPeriod >= 0;
    }

};

class TaskScheduler{

  private:
    static void cleanupTaskQueueElements();
    static TaskQueueElement *rootTaskQueueElement;
    static bool maybeDelete;

  public:
    static void scheduleRecurrentTask(RunnableTask *task, int period);
    static void scheduleRunOnceTask(RunnableTask *task, int period);
    static void removeTask(RunnableTask *task);
    static void rescheduleTask(RunnableTask *task, int period);
    static void process();
    static byte countQueue(){
      byte counter = 0;
      TaskQueueElement *current = TaskScheduler::rootTaskQueueElement;
      Serial.print("Tasks: ");
      while(current != NULL){
        counter++;
        Serial.print(current->task->taskName());
        Serial.print("(running: ");
        Serial.print(current->task->isTaskRunning());
        Serial.print(") ");
        current = current->nextTaskQueueElement;
      }
      Serial.println();
      return counter;
    }
};

TaskQueueElement *TaskScheduler :: rootTaskQueueElement = NULL;
bool TaskScheduler :: maybeDelete = false;

void TaskScheduler :: process(){
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  TaskQueueElement *nextQueueTask;
  unsigned long currentTime;
  TaskScheduler::maybeDelete = false;
  while(currentTaskQueueElement != NULL){
    currentTime = millis();
    nextQueueTask = currentTaskQueueElement->nextTaskQueueElement;

    // Time to run
    if (currentTime >= currentTaskQueueElement->nextRun){

      if (currentTaskQueueElement->task->isTaskRunning()){
        char * taskName = currentTaskQueueElement->task->taskName();
        if (! (strcmp(taskName, "IRCarControl")==0 || strcmp(taskName, "EventBus")==0) ){
          Serial.print("Executing ");
          Serial.println(taskName);
        }
        currentTaskQueueElement->task->execute();
      }

      if (currentTaskQueueElement->isRecurrent()){
        currentTaskQueueElement->nextRun = (currentTime + currentTaskQueueElement->recurrentPeriod);
      } else {
        delete(currentTaskQueueElement->task);
        currentTaskQueueElement->markForDeletion = true;
        TaskScheduler::maybeDelete = true;
      }
    }
    currentTaskQueueElement = nextQueueTask;
  }
  if (TaskScheduler::maybeDelete){
    TaskScheduler::cleanupTaskQueueElements();
  }
};

void TaskScheduler :: cleanupTaskQueueElements(){
  if (TaskScheduler::rootTaskQueueElement == NULL) return;
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  if (currentTaskQueueElement->markForDeletion){
    TaskScheduler::rootTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
    delete(currentTaskQueueElement);
  } 
  while(currentTaskQueueElement->nextTaskQueueElement != NULL){
    if (currentTaskQueueElement->nextTaskQueueElement->markForDeletion){
      TaskQueueElement *deleteTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
      currentTaskQueueElement->nextTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement->nextTaskQueueElement;
      delete(deleteTaskQueueElement);
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
  #if VERBOSE > 0
    TaskScheduler::countQueue();
  #endif
};

void TaskScheduler :: removeTask(RunnableTask *task){
  #if VERBOSE > 0
    Serial.println("Removing task");
  #endif
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  while(currentTaskQueueElement != NULL){
    if (currentTaskQueueElement->task == task){
      currentTaskQueueElement->task = NULL;
      currentTaskQueueElement->markForDeletion = true;
      TaskScheduler::maybeDelete = true;
      break;
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
}

void TaskScheduler :: rescheduleTask(RunnableTask *task, int period){
   #if VERBOSE > 0
    Serial.print("Rescheduling task to ");
    Serial.print(period);
    Serial.println("ms");
  #endif
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  while(currentTaskQueueElement != NULL){
    if (currentTaskQueueElement->task == task){
      currentTaskQueueElement->recurrentPeriod = period;
      currentTaskQueueElement->nextRun = 0;
      break;
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
}

void TaskScheduler :: scheduleRecurrentTask(RunnableTask *task, int period){
  #if VERBOSE > 0
    Serial.print("Scheduling recurrent task. ");
    Serial.print(period);
    Serial.println("ms ");
  #endif
  TaskScheduler::rootTaskQueueElement = new TaskQueueElement(task, period, TaskScheduler::rootTaskQueueElement);
};

void TaskScheduler :: scheduleRunOnceTask(RunnableTask *task, int period){
  #if VERBOSE > 0
    Serial.print("Scheduling run once task. ");
    Serial.print(period);
    Serial.println("ms ");
  #endif
  TaskScheduler::rootTaskQueueElement = new TaskQueueElement(task, -1, TaskScheduler::rootTaskQueueElement);
  TaskScheduler::rootTaskQueueElement->nextRun = millis() + period;
};

#endif
