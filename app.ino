#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "src/Robot.h"
#include "config.h"

#define DEBUG 1

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

  robot->home();

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

  if (DEBUG) {
    Serial.begin(115200);
    // Create debug task
    xTaskCreate(
      TaskDebug
      ,  (const portCHAR *)"Joystick"   // A name just for humans
      ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
      ,  NULL
      ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,  NULL );
    }

}

void loop(){}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskMotors(void *pvParameters) {
  (void) pvParameters;

  while(true) {
    robot->update();
  }
}

void TaskJoystick(void *pvParameters) {
  while(true) {
    robot->js->update();
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

void TaskDebug(void *pvParametes) {
  pinMode(LED_BUILTIN, OUTPUT);
  while(true) {
    digitalWrite(LED_BUILTIN, HIGH);

    short P0 = robot->motor0->getPos();
    short P1 = robot->motor1->getPos();
    unsigned short F0 = robot->motor0->getFrequency();
    unsigned short F1 = robot->motor1->getFrequency();
    bool D0 = robot->motor0->getDirection();
    bool D1 = robot->motor1->getDirection();
    short JX = robot->js->getX();
    short JY = robot->js->getY();

    Serial.print("T: ");
    Serial.print(millis());
    Serial.print(" P0:");
    Serial.print(P0);
    Serial.print(" P1:");
    Serial.print(P1);
    Serial.print(" F0:");
    if (D0 == CW) {
      Serial.print('-');
    }
    Serial.print(F0);
    Serial.print(" F1:");
    if (D1 == CW) {
      Serial.print('-');
    }
    Serial.print(F1);
    Serial.print(" JX:");
    Serial.print(JX);
    Serial.print(" JY:");
    Serial.println(JY);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
