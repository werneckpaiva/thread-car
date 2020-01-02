#define VERBOSE 0

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

  public:
    static void scheduleRecurrentTask(RunnableTask *task, int period);
    static void scheduleRunOnceTask(RunnableTask *task, int period);
    static void removeTask(RunnableTask *task);
    static void process();
    static byte countQueue(){
      byte counter = 0;
      TaskQueueElement *current = TaskScheduler::rootTaskQueueElement;
      while(current != NULL){
        counter++;
        current = current->nextTaskQueueElement;
      }
      return counter;
    }
};

TaskQueueElement *TaskScheduler :: rootTaskQueueElement = NULL;

void TaskScheduler :: process(){
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  TaskQueueElement *nextQueueTask;
  unsigned long currentTime;
  bool maybeDelete = false;
  while(currentTaskQueueElement != NULL){
    currentTime = millis();
    nextQueueTask = currentTaskQueueElement->nextTaskQueueElement;
    if (currentTime >= currentTaskQueueElement->nextRun){
//      Serial.print("CurrentTime: ");
//      Serial.print(currentTime);
//      Serial.print(" next run: ");
//      Serial.println(currentTaskQueueElement->nextRun);
      if (currentTaskQueueElement->task->isTaskRunning()){
        currentTaskQueueElement->task->execute(); 
      }
      if (currentTaskQueueElement->isRecurrent()){
        currentTaskQueueElement->nextRun = (currentTime + currentTaskQueueElement->recurrentPeriod);
      } else {
//        delete(currentTaskQueueElement->task);
        currentTaskQueueElement->markForDeletion = true;
      }
      maybeDelete = true;
    }
    currentTaskQueueElement = nextQueueTask;
  }
  if (maybeDelete){
    TaskScheduler::cleanupTaskQueueElements();
  }
};

void TaskScheduler :: cleanupTaskQueueElements(){
  if (TaskScheduler::rootTaskQueueElement == NULL) return;
  TaskQueueElement *currentTaskQueueElement = TaskScheduler::rootTaskQueueElement;
  if (currentTaskQueueElement->markForDeletion){
    TaskScheduler::rootTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
//    delete(currentTaskQueueElement);
  } 
  while(currentTaskQueueElement->nextTaskQueueElement != NULL){
    if (currentTaskQueueElement->nextTaskQueueElement->markForDeletion){
      TaskQueueElement *deleteTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
      currentTaskQueueElement->nextTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement->nextTaskQueueElement;
//      delete(deleteTaskQueueElement);
    }
    currentTaskQueueElement = currentTaskQueueElement->nextTaskQueueElement;
  }
  #if VERBOSE > 2
    Serial.print("Size of the execution queue: ");
    Serial.println(TaskScheduler::countQueue());
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
