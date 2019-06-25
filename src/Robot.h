#include <Arduino.h>
#include "Motor/Motor.h"
#include "Joystick/Joystick.h"

#define LINK0 80
#define LINK1 80

#define MINPOSRADIUS 50

class Robot {
    private:
        bool enabled;
        float xPos;
        float yPos;
        float xTarget;
        float yTarget;
    public:
        Robot(char[], char, char[], char, char[]);
        Motor *motor0;
        Motor *motor1;
        Joystick *js;

        void home();
        void behavior0();

        void joy2Target();
        void setMotors2Target();
        void moveMotors();

        float getXTarget();
        float getYTarget();

        void updatePos();
        float getXPos();
        float getYPos();
};
