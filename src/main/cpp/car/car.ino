#include "EventBus.h"
#include "CarEvents.h"
#include "ActionScheduler.h"
#include "CarMovement.h"
#include "CarControl.h"
#include "IRCarControl.h"


#define DIR_1_L_PIN      2    // Motor direction
#define DIR_2_L_PIN      4    // Motor direction
#define SPEED_L_PIN      6    // Needs to be a PWM pin to be able to control motor speed
#define DIR_1_R_PIN      7    // Motor direction
#define DIR_2_R_PIN      8    // Motor direction
#define SPEED_R_PIN      5    // Needs to be a PWM pin to be able to control motor speed


#define IR_PIN           3    // IR Port

CarMovement carMovement(DIR_1_L_PIN, DIR_2_L_PIN, SPEED_L_PIN,
                        DIR_1_R_PIN, DIR_2_R_PIN, SPEED_R_PIN);

EventBus eventBus;

CarControl carControl(&carMovement, &eventBus);

IRCarControl irCarControl(IR_PIN, &eventBus);


void setup(){
  Serial.begin(9600);
  carMovement.setup();
  carControl.setup();
  irCarControl.setup();
}

void loop() {
  ActionScheduler::updateActions();
}
