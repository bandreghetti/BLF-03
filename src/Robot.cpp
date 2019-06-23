#include "Robot.h"

Robot::Robot(
        char mot0Pins[],
        char mot0Endstop,
        char mot1Pins[],
        char mot1Endstop,
        char joyPorts[],
        char enablePort
    ) {
    this->motor0 = new Motor(mot0Pins, mot0Endstop);
    this->motor1 = new Motor(mot1Pins, mot1Endstop);
    this->js = new Joystick(joyPorts[0], joyPorts[1], enablePort);
}
