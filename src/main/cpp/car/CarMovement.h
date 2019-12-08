#ifndef CarMovement_h
#define CarMovement_h

class CarMovement {
  private:
    const int MIN_MOVING_SPEED = 90;
    const int MAX_MOVING_SPEED = 250;
    const int MOVING_SPEED_DIFF = (MAX_MOVING_SPEED - MIN_MOVING_SPEED);
  
    int dir1LeftPin;
    int dir2LeftPin;
    int speedLeftPin;
    int dir1RightPin;
    int dir2RightPin;
    int speedRightPin;

    int currentSpeed = MIN_MOVING_SPEED;

    void setMotorSpeed(int speedL,int speedR) {
        analogWrite(this->speedLeftPin, abs(speedL)); 
        analogWrite(this->speedRightPin, abs(speedR));   
    }

  public:

    CarMovement(int dir1LeftPin, int dir2LeftPin, int speedLeftPin,
                int dir1RightPin, int dir2RightPin, int speedRightPin){
      this->dir1LeftPin = dir1LeftPin;
      this->dir2LeftPin = dir1LeftPin;
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
      this->currentSpeed = movingSpeed;
      this->setMotorSpeed(movingSpeed, movingSpeed);
    }

    void stop() {
      setMovingSpeed(0);
      digitalWrite(this->dir1LeftPin, LOW);
      digitalWrite(this->dir2LeftPin, LOW);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void moveForward(int movingSpeed) {
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin, HIGH);
      digitalWrite(this->dir2LeftPin, LOW);
      digitalWrite(this->dir1RightPin, HIGH);
      digitalWrite(this->dir2RightPin, LOW);
    }

    void moveBackward(int movingSpeed) {
      setMovingSpeed(movingSpeed);
      digitalWrite(this->dir1LeftPin, LOW);
      digitalWrite(this->dir2LeftPin, HIGH);
      digitalWrite(this->dir1RightPin, LOW);
      digitalWrite(this->dir2RightPin, HIGH);
    }

    int getCurrentSpeed(){
      return currentSpeed;
    }

};

#endif
