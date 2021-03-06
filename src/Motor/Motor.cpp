#include "Motor.h"

Motor::Motor(char pins[], char endstop) {
    int i;
    for (i = 0; i < 4; ++i){
        this->pins[i] = pins[i];
        pinMode(this->pins[i], OUTPUT);
    }

    this->endstop = endstop;
    pinMode(endstop, INPUT_PULLUP);

    // Initialize motor in state 0
    this->state = 0;
	digitalWrite(this->pins[0], HIGH);
	digitalWrite(this->pins[1], LOW);
	digitalWrite(this->pins[2], LOW);
	digitalWrite(this->pins[3], HIGH);
    this->stopped = true;
    this->lastStepTime = micros();
}

bool Motor::endstopHit() {
    if (digitalRead(this->endstop)) {
        return false;
    } else {
        return true;
    }
}

bool Motor::zero() {
    int steps;

    // Approach endstop quickly
    this->setFrequency(MAX_FREQ/2);
    this->setDirection(CCW);
    for (steps = 0; steps < STEPS_PER_REV; ++steps) {
        if (this->endstopHit()) {
            // if motor has hit the endstop, break out of the loop
            break;
        }
        this->blockingStep();
    }
    if (steps == STEPS_PER_REV) {
        // if motor hasn't hit the endstop, return false indicating failure
        return false;
    }

    // Back off a little
    this->setDirection(CW);
    for (steps = 0; steps < STEPS_PER_REV/36; ++steps) {
        this->blockingStep();
    }
    if (this->endstopHit()) {
        // if endstop is still pressed after backing off
        return false;
    }

    // Approach endstop gently
    this->setFrequency(MAX_FREQ/4);
    this->setDirection(CCW);
    for (steps = 0; steps < STEPS_PER_REV/18; ++steps) {
        if (this->endstopHit()) {
            // if motor has hit the endstop, break out of the loop
            break;
        }
        this->blockingStep();
    }
    if (steps == STEPS_PER_REV/18) {
        // if motor hasn't hit the endstop, return false indicating failure
        return false;
    }

    return true;
}

void Motor::step(bool dir) {
  if(dir == CCW){
    if (this->state == 0) {
        this->state = 3;
    } else {
        this->state = this->state - 1;
    }
    this->pos += 1;
  } else if(dir == CW) {
    if (this->state == 3) {
        this->state = 0;
    } else {
        this->state = this->state + 1;
    }
    this->pos -= 1;
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

void Motor::blockingStep() {
    while(!this->checkAndStep());
}

void Motor::goTo(short newPos, unsigned short freq) {
    this->setFrequency(freq);
    if (newPos > this->pos) {
        this->setDirection(CCW);
    } else {
        this->setDirection(CW);
    }

    short stepsToTake = abs(newPos - this->pos);
    for(int i = 0; i < stepsToTake; ++i){
        this->blockingStep();
    }
}

bool Motor::checkAndStep() {
    // If motor is stopped, just return false and don't step
    if (this->stopped) {
        return false;
    }

    // If endstop is being pressed, ignore steps sent towards it
    if (this->endstopHit() && this->dir == CCW) {
        return true;
    }

    // Check if period has passed
    if (micros() - this->lastStepTime >= this->period) {
        this->step(this->dir);
        return true;
    }

    // If period has not passed, return false
    return false;
}

bool Motor::move2Dest() {
    if (this->pos != this->destPos) {
        this->checkAndStep();
    }

    // Check if motor finished movement
    if (this->pos != this->destPos) {
        return false;
    } else {
        return true;
    }
}

void Motor::setFrequency(unsigned short frequency) {
    // If frequency is 0, set stopped flag to avoid division by zero
    if (frequency == 0) {
        this->stopped = true;
        return;
    }

    // Make sure frequency does not exceed a maximum of 200Hz
    if (frequency > MAX_FREQ) {
        frequency = MAX_FREQ;
    }

    this->frequency = frequency;

    // Period in microsseconds
    this->period = 1000000/frequency;
    this->stopped = false;
    return;
}

unsigned short Motor::getFrequency() {
    if (this->stopped){
        return 0;
    }
    return this->frequency;
}

void Motor::setDirection(bool dir) {
    this->dir = dir;
    return;
}

bool Motor::getDirection() {
    return this->dir;
}

void Motor::setPos(short pos) {
    this->pos = pos;
    return;
}

short Motor::getPos() {
    return this->pos;
}

void Motor::setDestPos(short destPos) {
    this->destPos = destPos;
    return;
}

short Motor::getDestPos() {
    return this->destPos;
}

float Motor::getRadPos() {
    return (float)this->pos * ((2*PI) / STEPS_PER_REV);
}

float Motor::getDegPos() {
    return (float)this->pos * (360.0 / STEPS_PER_REV);
}

float Motor::getDegDestPos() {
    return (float)this->destPos * (360.0 / STEPS_PER_REV);
}
