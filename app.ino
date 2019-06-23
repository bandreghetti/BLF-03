#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "src/Robot.h"
#include "config.h"

Robot *robot;

void TaskStep(void *pvParameters);
void TaskJoystick(void *pvParameters);

void setup() {

  Serial.begin(115200);

  char mot0Pins[] = MOT0PINS;
  char mot1Pins[] = MOT1PINS;
  char jsPorts[] = {JOYX, JOYY};

  robot = new Robot(
    mot0Pins,
    MOT0ENDSTOP,
    mot1Pins,
    MOT1ENDSTOP,
    jsPorts,
    ENABLEPORT
  );

  if (!robot->home()) {
    pinMode(LED_BUILTIN, OUTPUT);
    while(true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  // Create motors task
  xTaskCreate(
    TaskMotors
    ,  (const portCHAR *)"Motors"   // A name just for humans
    ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  // Create joystick task
  xTaskCreate(
    TaskJoystick
    ,  (const portCHAR *)"Joystick"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
}

void loop(){}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskMotors(void *pvParameters) {
  (void) pvParameters;

  robot->behavior0();
}

void TaskJoystick(void *pvParameters) {
  while(1) {
    robot->js->update();
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}
