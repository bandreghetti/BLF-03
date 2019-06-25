#include "Robot.h"

Robot::Robot(
        char mot0Pins[],
        char mot0Endstop,
        char mot1Pins[],
        char mot1Endstop,
        char joyPorts[]
    ) {
    this->motor0 = new Motor(mot0Pins, mot0Endstop);
    this->motor1 = new Motor(mot1Pins, mot1Endstop);
    this->js = new Joystick(joyPorts[0], joyPorts[1]);
}

void Robot::home() {
    if(!this->motor0->zero()) {
        pinMode(LED_BUILTIN, OUTPUT);
        while(true) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
        }
    }
    this->motor0->setPos(STEPS_PER_REV/2);

    if(!this->motor1->zero()) {
        pinMode(LED_BUILTIN, OUTPUT);
        while(true) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
        }
    }
    this->motor1->setPos(0);

    // this->motor0->goTo(STEPS_PER_REV/4, MAX_FREQ/2);

    this->motor0->setFrequency(0);
    this->motor1->setFrequency(0);
}

void Robot::behavior0() {
    while(1) {
        this->motor0->checkAndStep();
        this->motor1->checkAndStep();
        short newSpeed0 = this->js->getX();
        if (newSpeed0 < 0) {
          this->motor0->setDirection(CCW);
        } else {
          this->motor0->setDirection(CW);
        }
        unsigned short newFreq0 = map(abs(newSpeed0), 0, 512, 0, 150);
        this->motor0->setFrequency(newFreq0);

        short newSpeed1 = this->js->getY();
        if (newSpeed1 > 0) {
          this->motor1->setDirection(CCW);
        } else {
          this->motor1->setDirection(CW);
        }
        unsigned short newFreq1 = map(abs(newSpeed1), 0, 512, 0, 150);
        this->motor1->setFrequency(newFreq1);
  }
}

void Robot::joy2Target() {
    this->js->update();
    short joyX = this->js->getRawX();
    short joyY = this->js->getRawY();

    const float armLen = (float)(LINK0+LINK1);
    float xTarget = joyX * (2*armLen / 1023.0) - armLen;
    float yTarget = joyY * armLen / 1023.0;

    const float posRad = sqrt(pow(xTarget, 2) + pow(yTarget, 2));
    const bool exceedsArmLength = posRad > armLen;
    const bool tooCloseToOrigin = posRad < MINPOSRADIUS;
    const bool invalidElbow = sqrt(pow(xTarget+LINK0, 2) + pow(yTarget, 2)) < LINK1;

    // If target position exceeds arm length
    const float angleZero2Target = atan2(yTarget, xTarget);
    if (exceedsArmLength) {
        xTarget = armLen*cos(angleZero2Target);
        yTarget = armLen*sin(angleZero2Target);
    // Fix blind spots
    } else if (tooCloseToOrigin || invalidElbow) {
        // Formulas derived in http://mathworld.wolfram.com/Circle-CircleIntersection.html
        const float crossingX = -((LINK0*LINK0) - (LINK1*LINK1) + (MINPOSRADIUS*MINPOSRADIUS))/(float)(2*LINK0);
        const float crossingY = sqrt((long)(-LINK0+LINK1+MINPOSRADIUS)
                                    *(long)(+LINK0+LINK1-MINPOSRADIUS)
                                    *(long)(+LINK0-LINK1+MINPOSRADIUS)
                                    *(long)(+LINK0+LINK1+MINPOSRADIUS))/(float)(2*LINK0);
        const float angleLeft2Target = atan2(yTarget, xTarget+LINK0);
        const float angleZero2Crossing = atan2(crossingY, crossingX);
        const float angleLeft2Crossing = atan2(crossingY, crossingX+LINK0);

        // If point would be put inside the other restricting semicircle
        if (angleZero2Target > angleZero2Crossing &&
            angleLeft2Target < angleLeft2Crossing) {
                xTarget = crossingX;
                yTarget = crossingY;
        // If point is inside semicircle around origin
        } else if (tooCloseToOrigin) {
            xTarget = MINPOSRADIUS*cos(angleZero2Target);
            yTarget = MINPOSRADIUS*sin(angleZero2Target);
        // If point is inside non-bending elbow limitation semicircle
        } else if (invalidElbow) {
            xTarget = LINK1*cos(angleLeft2Target)-LINK0;
            yTarget = LINK1*sin(angleLeft2Target);
        }
    }

    this->xTarget = xTarget;
    this->yTarget = yTarget;
    return;
}

void Robot::setMotors2Target() {
    // Calculate Theta1
    const float theta1 = -acos((pow(this->xTarget, 2) + pow(this->yTarget, 2) - LINK0*LINK0 - LINK1*LINK1)
                           /(float)(2*LINK0*LINK1)
                           );
    const short destPos1 = (short)(theta1 * STEPS_PER_REV / (2*PI));

    // Calculate Theta0
    const float sinTheta1 = sin(theta1);
    const float cosTheta1 = cos(theta1);
    const float n = this->yTarget*(LINK0+LINK1*cosTheta1) - this->xTarget*LINK1*sinTheta1;
    const float d = this->xTarget*(LINK0+LINK1*cosTheta1) + this->yTarget*LINK1*sinTheta1;
    const float theta0 = atan2(n, d);
    const short destPos0 = (short)(theta0 * STEPS_PER_REV / (2*PI));;

    // Calculate how many steps each motor needs to turn
    const short delta0 = destPos0 - this->motor0->getPos();
    const short delta1 = destPos1 - this->motor1->getPos();

    short freq0, freq1;

    // If there is no need to move, just return
    if (delta0 == 0 || delta1 == 0) return;

    // Calculate motor frequencies
    if (abs(delta0) > abs(delta1)) {
        freq0 = MAX_FREQ;
        freq1 = MAX_FREQ*((float)abs(delta1)/abs(delta0));
    } else {
        freq1 = MAX_FREQ;
        freq0 = MAX_FREQ*((float)abs(delta0)/abs(delta1));
    }


    // Configure Motor0
    if (delta0 > 0) {
        this->motor0->setDirection(CCW);
    } else {
        this->motor0->setDirection(CW);
    }
    this->motor0->setDestPos(destPos0);
    this->motor0->setFrequency(freq0);

    // Configure Motor1
    if (delta1 > 0) {
        this->motor1->setDirection(CCW);
    } else {
        this->motor1->setDirection(CW);
    }
    this->motor1->setDestPos(destPos1);
    this->motor1->setFrequency(freq1);
}

void Robot::updatePos() {
    const float theta0 = this->motor0->getRadPos();
    const float theta1 = this->motor1->getRadPos();

    this->xPos = LINK0*cos(theta0) + LINK1*cos(theta0+theta1);
    this->yPos = LINK0*sin(theta0) + LINK1*sin(theta0+theta1);
}

bool Robot::move2Dest() {
    const bool motor0Finished = this->motor0->move2Dest();
    const bool motor1Finished = this->motor1->move2Dest();
    if (motor0Finished && motor1Finished) {
        return true;
    }
    return false;
}

float Robot::getXTarget() {
    return this->xTarget;
}

float Robot::getYTarget() {
    return this->yTarget;
}

float Robot::getXPos() {
    return this->xPos;
}

float Robot::getYPos() {
    return this->yPos;
}
