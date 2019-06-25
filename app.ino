#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "src/Robot.h"
#include "config.h"

#define DEBUG 1

Robot *robot;

SemaphoreHandle_t jsMutex;

void TaskJoystick(void *pvParameters);
void TaskSetMotors(void *pvParameters);
void TaskMoveMotors(void *pvParameters);
void TaskDebug(void *pvParameters);

void setup() {
  char mot0Pins[] = MOT0PINS;
  char mot1Pins[] = MOT1PINS;
  char jsPorts[] = {JOYX, JOYY};

  if (jsMutex == NULL) {
    jsMutex = xSemaphoreCreateBinary();
    if (jsMutex != NULL) {
      xSemaphoreGive(jsMutex);
    }
  }

  robot = new Robot(
    mot0Pins,
    MOT0ENDSTOP,
    mot1Pins,
    MOT1ENDSTOP,
    jsPorts
  );

  robot->home();

  // Create Joystick task
  xTaskCreate(
    TaskJoystick
    ,  (const portCHAR *)"Joystick"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
  );

  // Create setMotors task
  xTaskCreate(
    TaskSetMotors
    ,  (const portCHAR *)"SetMotors"   // A name just for humans
    ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
  );

  // Create moveMotors task
  xTaskCreate(
    TaskMoveMotors
    ,  (const portCHAR *)"MoveMotors"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
  );

  if (DEBUG) {
    Serial.begin(115200);
    // Create debug task
    xTaskCreate(
      TaskDebug
      ,  (const portCHAR *)"Debug"   // A name just for humans
      ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
      ,  NULL
      ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,  NULL );
    }

}

void loop(){}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskMoveMotors(void *pvParameters) {
  (void) pvParameters;
  while(true) {
    robot->moveMotors();
  }
}

void TaskSetMotors(void *pvParameters) {
  (void) pvParameters;

  while(true) {
    xSemaphoreTake(jsMutex, portMAX_DELAY);
    robot->setMotors2Target();
  }
}

void TaskJoystick(void *pvParameters) {
  (void) pvParameters;

  while(true) {
    robot->joy2Target();
    xSemaphoreGive(jsMutex);
    vTaskDelay((500) / portTICK_PERIOD_MS);
  }
}

void TaskDebug(void *pvParametes) {
  pinMode(LED_BUILTIN, OUTPUT);
  while(true) {
    digitalWrite(LED_BUILTIN, HIGH);

    robot->updatePos();

    Serial.print("T:");
    Serial.println(millis());

    Serial.print("A0:");
    Serial.print(robot->motor0->getDegPos());
    Serial.print(" T0:");
    Serial.println(robot->motor0->getDegDestPos());

    Serial.print("A1:");
    Serial.print(robot->motor1->getDegPos());
    Serial.print(" T1:");
    Serial.println(robot->motor1->getDegDestPos());

    Serial.print("X:");
    Serial.print(robot->getXPos());
    Serial.print(" XT:");
    Serial.println(robot->getXTarget());

    Serial.print("Y:");
    Serial.print(robot->getYPos());
    Serial.print(" YT:");
    Serial.println(robot->getYTarget());

    Serial.println();

    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
