#include "EventBus.h"
#include "CarEvents.h"
#include "ActionScheduler.h"
#include "CarStates.h"
#include "CarMovement.h"
#include "CarModuleControl.h"
#include "IRCarControl.h"
#include "SerialCarControl.h"
#include "DistanceDetector.h"
#include "DistanceDetectionState.h"
#include "AutoPilot.h"


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

EventBus eventBus;

CarModuleControl carMovementControl(&eventBus, 
  new StoppedCarMovementState(
    new CarMovementStateControl(&carMovement, &eventBus)));

DistanceDetector distanceDetector(SERVO_PIN, ECHO_PIN, TRIG_PIN);
CarModuleControl distanceDetectionControl(&eventBus, 
  new StoppedDistanceDetectionState(
     new DistanceDetectionStateControl(&distanceDetector, &eventBus)));

IRCarControl irCarControl(IR_PIN, &eventBus);
SerialCarControl serialCarControl(&eventBus);

CarModuleControl autoPilot(&eventBus, new MonitoringAutoPilotState(&eventBus));

void setup(){
  serialCarControl.setup();
  irCarControl.setup();

  carMovement.setup();
  distanceDetector.setup();
  carMovementControl.setup();
  autoPilot.setup();
}

void loop() {
  ActionScheduler::updateActions();
  TaskScheduler::process();

}
