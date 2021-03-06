#include "Joystick.h"

Joystick::Joystick(char xPort, char yPort) {
    this->xPort = xPort;
    this->yPort = yPort;
    pinMode(xPort, INPUT);
    pinMode(yPort, INPUT);
    this->centerX = analogRead(xPort);
    this->centerY = analogRead(yPort);
}

void Joystick::update() {
    int newX, newY;

    // Update X reading
    newX = analogRead(this->xPort);
    if(newX < this->centerX + DEADZONE
    && newX > this->centerX - DEADZONE) {
        this->x = 0;
    } else {
        if(newX > this->centerX) {
            this->x = map(newX, this->centerX+DEADZONE, 1023, 0, 512);
        } else {
            this->x = map(newX, this->centerX-DEADZONE, 0, 0, -512);
        }
    }
    this->rawX = newX;

    // Update Y reading
    newY = 1023-analogRead(this->yPort);
    if(newY < this->centerY + DEADZONE
    && newY > this->centerY - DEADZONE) {
        this->y = 0;
    } else {
        if(newY > this->centerY) {
            this->y = map(newY, this->centerY+DEADZONE, 1023, 0, 512);
        } else {
            this->y = map(newY, this->centerY-DEADZONE, 0, 0, -512);
        }
    }
    this->rawY = newY;
}

short Joystick::getRawX() {
    return this->rawX;
}

short Joystick::getRawY() {
    return this->rawY;
}

short Joystick::getX() {
    return this->x;
}

short Joystick::getY() {
    return this->y;
}
