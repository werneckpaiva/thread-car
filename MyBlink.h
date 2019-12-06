#include <VariableTimedAction.h>

#ifndef MyBlink_h
#define MyBlink_h

class MyBlink : public VariableTimedAction {
  private:
    int pin;
    bool on = false;
    int i = 0;
    bool isFastSpeed = false;
   
    unsigned long run() {
      if (!on) {
        digitalWrite(pin, HIGH);
      } else {
        digitalWrite(pin, LOW);
      }
      on = !on;
      //returns the amount in seconds to wait before executing the next event
      //if 0 is returned, then the previous interval is maintained
  
      i = (i + 1) % 10;
      if (i==0){
        isFastSpeed = ! isFastSpeed;
        return (isFastSpeed)? 500:100;
      }
      return 0;
    }
  public:
    MyBlink(int p) {
        pin = p;
      pinMode(pin, OUTPUT);
    }
  
    void init(){
      start(100);
    }
};

#endif
