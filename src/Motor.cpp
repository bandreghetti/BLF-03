#include <Arduino.h>
#include "Motor.h"

Motor::Motor(char pins[]) {
    int i;
    for (i = 0; i < 4; ++i){
        this->pins[i] = pins[i];
        pinMode(this->pins[i], OUTPUT);
    }

    // Initialize motor in state 0
    this->state = 0;
	digitalWrite(this->pins[0], HIGH);
	digitalWrite(this->pins[1], LOW);
	digitalWrite(this->pins[2], LOW);
	digitalWrite(this->pins[3], HIGH);
    this->stopped = true;
    this->lastStepTime = micros();
}

void Motor::step(bool dir) {
  if(dir == CCW){
    if (this->state == 0) {
        this->state = 3;
    } else {
        this->state = this->state - 1;
    }
  } else if(dir == CW) {
    if (this->state == 3) {
        this->state = 0;
    } else {
        this->state = this->state + 1;
    }
  }
  switch(this->state) {
    case 0:
      digitalWrite(this->pins[0], HIGH);
      digitalWrite(this->pins[1], LOW);
      digitalWrite(this->pins[2], LOW);
      digitalWrite(this->pins[3], HIGH);
      break;
    case 1:
      digitalWrite(this->pins[0], HIGH);
      digitalWrite(this->pins[1], HIGH);
      digitalWrite(this->pins[2], LOW);
      digitalWrite(this->pins[3], LOW);
      break;
    case 2:
      digitalWrite(this->pins[0], LOW);
      digitalWrite(this->pins[1], HIGH);
      digitalWrite(this->pins[2], HIGH);
      digitalWrite(this->pins[3], LOW);
      break;
    case 3:
      digitalWrite(this->pins[0], LOW);
      digitalWrite(this->pins[1], LOW);
      digitalWrite(this->pins[2], HIGH);
      digitalWrite(this->pins[3], HIGH);
      break;
  }
  this->lastStepTime = micros();
}

bool Motor::checkAndStep() {
    // If motor is stopped, just return false and don't step
    if (this->stopped) {
        return false;
    }

    // Check if period has passed
    if (micros() - this->lastStepTime >= this->period) {
        this->step(this->dir);
        return true;
    }

    // If period has not passed, return false
    return false;
}

void Motor::setFrequency(unsigned short frequency) {
    // If frequency is 0, set stopped flag to avoid division by zero
    if (frequency == 0) {
        this->stopped = true;
        return;
    }

    // Make sure frequency does not exceed a maximum of 200Hz
    if (frequency > MAX_FREQ) {
        frequency = 200;
    }

    // Period in microsseconds
    this->period = 1000000/frequency;
    this->stopped = false;
    return;
}

void Motor::setDirection(bool dir) {
    this->dir = dir;
    return;
}
