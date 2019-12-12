#include <Servo.h>

#ifndef DistanceDetectionStates_h
#define DistanceDetectionStates_h

class DistanceDetector{
  private:
    byte servoPin;
    byte echoPin;
    byte triggerPin;

    Servo head;
    byte headAngle = 90;

  public:
    DistanceDetector(byte servoPin, byte echoPin, byte triggerPin){
      this->servoPin = servoPin;
      this->echoPin = echoPin;
      this->triggerPin = triggerPin;
    }

    void setup(){
      pinMode(this->triggerPin, OUTPUT); 
      pinMode(this->echoPin, INPUT);

      // Init Servo
      head.attach(this->servoPin); 
      head.write(headAngle);
    }
};

#endif
