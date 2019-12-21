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
    static const double SOUND_SPEED = 0.01657;

  public:
    static const byte MAX_DISTANCE = 100;
    static const byte MIN_DISTANCE = 4;
    static const byte MIN_HEAD_ANGLE = 0;
    static const byte MAX_HEAD_ANGLE = 180;
    static const byte MID_HEAD_ANGLE = (MIN_HEAD_ANGLE + MAX_HEAD_ANGLE) / 2;
    
    DistanceDetector(byte servoPin, byte echoPin, byte triggerPin){
      this->servoPin = servoPin;
      this->echoPin = echoPin;
      this->triggerPin = triggerPin;
    };

    void setup(){
      pinMode(this->triggerPin, OUTPUT); 
      pinMode(this->echoPin, INPUT);

      // Init Servo
      head.attach(this->servoPin); 
      head.write(headAngle);
    };

    void moveHead(byte angle){
      if (this->headAngle == angle) {
        return;
      }
      if (angle < DistanceDetector::MIN_HEAD_ANGLE) {
        angle = DistanceDetector::MIN_HEAD_ANGLE;
      }
      if (angle > DistanceDetector::MAX_HEAD_ANGLE) {
        angle = DistanceDetector::MAX_HEAD_ANGLE;
      }
      this->headAngle = angle;
      head.write (angle);
    };

    byte detectDistance(){
      long duration;
      byte distance;
      digitalWrite(this->triggerPin, LOW);
      delayMicroseconds(5);
      digitalWrite(this->triggerPin, HIGH);
      delayMicroseconds(15);
      digitalWrite(this->triggerPin, LOW);
      duration = pulseIn(this->echoPin, HIGH, 5000);
      if (duration <= 5){
        return DistanceDetector::MAX_DISTANCE;
      }
      distance = round(duration * DistanceDetector::SOUND_SPEED);
      if (distance > DistanceDetector::MAX_DISTANCE) return DistanceDetector::MAX_DISTANCE;
      if (distance < DistanceDetector::MIN_DISTANCE) return DistanceDetector::MIN_DISTANCE;
      return distance;
    }
    
};

#endif
