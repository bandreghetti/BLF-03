#include <Arduino_FreeRTOS.h>

#define CW 0
#define CCW 1

void initStepper(byte out[4]);
void oneStep(byte pin[4], byte* current_state, bool dir);

byte current_state[] = {0, 0};
byte out0[] = {8, 9, 10, 11};

// define two tasks for Blink & AnalogRead
void TaskStep( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  initStepper(out0);

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskStep
    ,  (const portCHAR *)"Step"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
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

void TaskStep(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  while(1) {
    for (int i = 0; i < 2048; ++i)
    {
        if (i%16 == 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            vTaskDelay(1);
            digitalWrite(LED_BUILTIN, LOW);
        }
        oneStep(out0, &current_state[0], CCW);
        delay(4);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
  }

}

void oneStep(byte pin[4], byte* current_state, bool dir) {
  if(dir == CCW){
    switch(*current_state) {
      case 0:
        *current_state = 3;
        break;
      case 1:
        *current_state = 0;
        break;
      case 2:
        *current_state = 1;
        break;
      case 3:
        *current_state = 2;
        break;
    }
  } else if(dir == CW) {
    switch(*current_state) {
      case 0:
        *current_state = 1;
        break;
      case 1:
        *current_state = 2;
        break;
      case 2:
        *current_state = 3;
        break;
      case 3:
        *current_state = 0;
        break;
    }
  }
  switch(*current_state) {
    case 0:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], HIGH);
      break;
    case 1:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], HIGH);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], LOW);
      break;
    case 2:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], HIGH);
      digitalWrite(pin[2], HIGH);
      digitalWrite(pin[3], LOW);
      break;
    case 3:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], HIGH);
      digitalWrite(pin[3], HIGH);
      break;
  }
}

void initStepper(byte out[4]) {
	pinMode(out[0], OUTPUT);
	pinMode(out[1], OUTPUT);
	pinMode(out[2], OUTPUT);
	pinMode(out[3], OUTPUT);
	digitalWrite(out[0], HIGH);
	digitalWrite(out[1], LOW);
	digitalWrite(out[2], LOW);
	digitalWrite(out[3], HIGH);
	delay(100);
}
