#include "Joystick.h"

Joystick::Joystick(char xPort, char yPort, char ePort) {
    this->xPort = xPort;
    this->yPort = yPort;
    this->ePort = ePort;
    pinMode(xPort, INPUT);
    pinMode(yPort, INPUT);
    pinMode(ePort, INPUT);
}

void Joystick::update() {
    int newX, newY;

    // Update X reading
    newX = analogRead(this->xPort);
    if(newX < CENTER + DEADZONE
    && newX > CENTER - DEADZONE) {
        this->x = 0;
    } else {
        if(newX > CENTER) {
            this->x = map(newX, CENTER+DEADZONE, 1023, 0, CENTER);
        } else {
            this->x = map(newX, CENTER-DEADZONE, 0, 0, -CENTER);
        }
    }

    // Update Y reading
    newY = analogRead(this->yPort);
    if(newY < CENTER + DEADZONE
    && newY > CENTER - DEADZONE) {
        this->y = 0;
    } else {
        if(newY > CENTER) {
            this->y = map(newY, CENTER+DEADZONE, 1023, 0, -CENTER);
        } else {
            this->y = map(newY, CENTER-DEADZONE, 0, 0, +CENTER);
        }
    }

    // Update enable port
    this->e = digitalRead(this->ePort);
}

short Joystick::getX() {
    return this->x;
}

short Joystick::getY() {
    return this->y;
}

bool Joystick::enabled() {
    return this->e;
}
