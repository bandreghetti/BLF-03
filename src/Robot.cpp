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

void Robot::update() {
    // Wait until next step
    while(!(this->motor0->checkAndStep() || this->motor1->checkAndStep()));

    // Convert motors positions in steps to angular positions in radians
    float theta0 = this->motor0->getPos() * 2*PI / STEPS_PER_REV;
    float theta1 = this->motor1->getPos() * 2*PI / STEPS_PER_REV;

    // Calculate linear position based on motor angular positions
    this->xPos = LINK0*cos(theta0) + LINK1*cos(theta0 + theta1);
    this->yPos = LINK0*sin(theta0) + LINK1*sin(theta0 + theta1);

    // Calculate desired velocity based on joystick input
    this->xSpd = this->js->getX()*JOY2SPD;
    this->ySpd = this->js->getY()*JOY2SPD;

    // Calculate motor1 angular velocity
    float omega1 = (this->xPos*this->xSpd + this->yPos*this->ySpd)/(float)(LINK0*LINK1);
    omega1 /= sqrt(1 + pow(pow(this->xPos, 2) + pow(this->yPos, 2) - LINK0*LINK0 - LINK1*LINK1, 2));

    // Calculate motor0 angular velocity
    float sinTheta1 = sin(theta1);
    float cosTheta1 = cos(theta1);
    float n = this->yPos*(LINK0+LINK1*cosTheta1) - this->xPos*LINK1*sinTheta1;
    float d = this->xPos*(LINK0+LINK1*cosTheta1) - this->yPos*LINK1*sinTheta1;
    float omega0 = d*(
        this->ySpd*(LINK0+LINK1*cosTheta1)
        -this->yPos*omega1*LINK1*sinTheta1
        -this->xSpd*LINK1*sinTheta1
        -this->xPos*omega1*LINK1*cosTheta1
        );
    omega0 -= n*(
        this->xSpd*(LINK0+LINK1*cosTheta1)
        -this->xPos*omega1*LINK1*sinTheta1
        +this->ySpd*LINK1*sinTheta1
        +this->yPos*omega1*LINK1*cosTheta1
    );
    omega0 *= pow(d, -2);
    omega0 /= 1 + pow(n/d, 2);

    // Convert angular velocities in rads/sec to motor frequencies in steps/sec
    short mot0Freq = (short)(omega0 * STEPS_PER_REV / (2*PI));
    short mot1Freq = (short)(omega1 * STEPS_PER_REV / (2*PI));

    // Set motor0 frequency and direction
    this->motor0->setFrequency(abs(mot0Freq));
    if (mot0Freq > 0) {
        this->motor0->setDirection(CCW);
    } else {
        this->motor0->setDirection(CW);
    }

    // Set motor1 frequency and direction
    this->motor0->setFrequency(abs(mot0Freq));
    if (mot0Freq > 0) {
        this->motor0->setDirection(CCW);
    } else {
        this->motor0->setDirection(CW);
    }
}
