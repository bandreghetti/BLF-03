#include <Arduino_FreeRTOS.h>
#include "src/Motor.h"
#include "config.h"

// define two tasks for Blink & AnalogRead
void TaskStep( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskMotors
    ,  (const portCHAR *)"Step"   // A name just for humans
    ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskMotors(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  char mot0pins[] = MOT0PINS;
  Motor mot0 = Motor(mot0pins);

  mot0.setFrequency(200);

  while(1) {
    for (int i = 0; i < 2048; ++i)
    {
      while(!mot0.checkAndStep());
    }
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
