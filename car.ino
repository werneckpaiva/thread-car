#include <VariableTimedAction.h>
#include "CarMovement.h"
#include "CarControl.h"

#define DIR_1_L_PIN      2    // Motor direction
#define DIR_2_L_PIN      4    // Motor direction
#define SPEED_L_PIN      6    // Needs to be a PWM pin to be able to control motor speed
#define DIR_1_R_PIN      7    // Motor direction
#define DIR_2_R_PIN      8    // Motor direction
#define SPEED_R_PIN      5    // Needs to be a PWM pin to be able to control motor speed

CarMovement carMovement(DIR_1_L_PIN, DIR_2_L_PIN, SPEED_L_PIN, 
                        DIR_1_R_PIN, DIR_2_R_PIN, SPEED_R_PIN);

CarControl carControl(&carMovement);

void setup(){
  carMovement.setup();
  carControl.setup();
}

void loop() {
  VariableTimedAction::updateActions();
}
