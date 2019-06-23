#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "src/Robot.h"
#include "config.h"

Robot *robot;

void TaskStep(void *pvParameters);
void TaskJoystick(void *pvParameters);

void setup() {

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

  while(1) {
    for (int i = 0; i < 2048; ++i)
    {
      while(!robot->motor0->checkAndStep()){
        short newSpeed = robot->js->getX();
        if (newSpeed > 0) {
          robot->motor0->setDirection(CCW);
        } else {
          robot->motor0->setDirection(CW);
        }

        unsigned short newFreq = map(abs(newSpeed), 0, 512, 0, 200);

        robot->motor0->setFrequency(newFreq);
      };
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void TaskJoystick(void *pvParameters) {
  while(1) {
    robot->js->update();
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}
