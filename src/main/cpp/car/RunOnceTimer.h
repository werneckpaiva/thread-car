  #include "ActionScheduler.h"

#ifndef RunOnceTimer_h
#define RunOnceTimer_h

class RunnableTask {
  public:
    virtual void execute();
};

class RunTimer : public ActionScheduler {

  protected:
    RunnableTask *currentTask = NULL;

  private:
    unsigned long run() {
      if (this->currentTask != NULL){
        Serial.println("Execute!!");
        currentTask->execute();
      }
      return 0;
    };

  public:
    void scheduleTask(RunnableTask *task, int interval){
      this->currentTask = task;
      this->continueRunning(interval);
    };

    void cancelSchedule() {
      this->currentTask = NULL;
      this->pause();
    };
};

class RunOnceTimer : public RunTimer {

  private:
    unsigned long run() {
      if (this->currentTask != NULL){
        currentTask->execute();
      }
      this->cancelSchedule();
      return 0;
    };
};

#endif
