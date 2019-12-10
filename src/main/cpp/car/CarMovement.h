#ifndef CarMovement_h
#define CarMovement_h

class CarMovement {
  private:
    static const int MIN_MOTOR_SPEED = 90;
    static const int MAX_MOTOR_SPEED = 250;
  
    int dir1LeftPin;
    int dir2LeftPin;
    int speedLeftPin;
    int dir1RightPin;
    int dir2RightPin;
    int speedRightPin;

    int currentSpeed = 0;

    void setMotorSpeed(int speedL,int speedR) {
        analogWrite(this->speedLeftPin, abs(speedL)); 
        analogWrite(this->speedRightPin, abs(speedR));   
    }

  public:

    static const int MAX_SPEED = (MAX_MOTOR_SPEED - MIN_MOTOR_SPEED);

    CarMovement(int dir1LeftPin, int dir2LeftPin, int speedLeftPin,
                int dir1RightPin, int dir2RightPin, int speedRightPin){
      this->dir1LeftPin = dir1LeftPin;
      this->dir2LeftPin = dir2LeftPin;
      this->speedLeftPin = speedLeftPin;
      this->dir1RightPin = dir1RightPin;
      this->dir2RightPin = dir2RightPin;
      this->speedRightPin = speedRightPin;
    }

    void setup(){
      pinMode(this->dir1LeftPin, OUTPUT);
      pinMode(this->dir2LeftPin, OUTPUT);
      pinMode(this->speedLeftPin, OUTPUT);
      pinMode(this->dir1RightPin, OUTPUT);
      pinMode(this->dir2RightPin, OUTPUT);
      pinMode(this->speedRightPin, OUTPUT);
    }

    void setMovingSpeed(int movingSpeed) {
      int realSpeed = MIN_MOTOR_SPEED + movingSpeed;
      if (realSpeed > MAX_MOTOR_SPEED){
        realSpeed = MAX_MOTOR_SPEED;
        movingSpeed = MAX_SPEED;
      }
      if (realSpeed <= MIN_MOTOR_SPEED){
        realSpeed = 0;
        movingSpeed = 0;
      }
      this->currentSpeed = movingSpeed;
      this->setMotorSpeed(realSpeed, realSpeed);
    }

    void stop() {
      setMovingSpeed(0);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void moveForward(int movingSpeed) {
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  HIGH);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, HIGH);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void moveBackward(int movingSpeed) {
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  HIGH);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, HIGH);
    }

    void spinRight(int movingSpeed){
      setMovingSpeed(MAX_MOTOR_SPEED);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  HIGH);
      digitalWrite(this->dir1RightPin, HIGH);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void spinLeft(int movingSpeed){
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  HIGH);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, HIGH);
    }

    void turnRightForward(int movingSpeed){
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, HIGH);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void turnLeftForward(int movingSpeed){
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  HIGH);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void turnRightBackward(int movingSpeed){
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  LOW);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, HIGH);
    }

    void turnLeftBackward(int movingSpeed){
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin,  LOW);
      digitalWrite(this->dir2LeftPin,  HIGH);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, LOW);
    }

    int getCurrentSpeed(){
      return currentSpeed;
    }

};

#endif
