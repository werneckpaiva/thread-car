//#include <ESP8266WiFi.h>
#include "EventBus.h"
#include "CarEvents.h"
#include "CarStates.h"
#include "CarMovement.h"
#include "CarModuleControl.h"
#include "IRCarControl.h"
//#include "SerialCarControl.h"
#include "DistanceDetector.h"
#include "DistanceDetectionState.h"
#include "AutoPilot.h"

#define VERBOSE 1


#define DIR_1_L_PIN      2    // Motor direction
#define DIR_2_L_PIN      4    // Motor direction
#define SPEED_L_PIN      6    // Needs to be a PWM pin to be able to control motor speed
#define DIR_1_R_PIN      7    // Motor direction
#define DIR_2_R_PIN      8    // Motor direction
#define SPEED_R_PIN      5    // Needs to be a PWM pin to be able to control motor speed

#define IR_PIN           3    // IR Port

#define ECHO_PIN         11   // Ultrasonic Echo
#define TRIG_PIN         12   // Ultrasonic Trig

#define SERVO_PIN        9 

CarMovement carMovement(DIR_1_L_PIN, DIR_2_L_PIN, SPEED_L_PIN,
                        DIR_1_R_PIN, DIR_2_R_PIN, SPEED_R_PIN);

CarModuleControl carMovementControl(new StoppedCarMovementState(
    new CarMovementStateControl(&carMovement)));

DistanceDetector distanceDetector(SERVO_PIN, ECHO_PIN, TRIG_PIN);
CarModuleControl distanceDetectionControl(new StoppedDistanceDetectionState(
     new DistanceDetectionStateControl(&distanceDetector)));

IRCarControl irCarControl(IR_PIN);
//SerialCarControl serialCarControl;

CarModuleControl autoPilot(new DrivingAutoPilotState());

void setup(){
  Serial.begin(74880);
  #if VERBOSE > 0
    Serial.println(">>>>>>>>> FROM THE BEGINNING");
  #endif
  EventBus::setup();

//  serialCarControl.setup();
  irCarControl.setup();

  carMovement.setup();
  carMovementControl.setup();
  
  distanceDetector.setup();
  autoPilot.setup();
}

void loop() {
  TaskScheduler::process();
}                                                                                              
