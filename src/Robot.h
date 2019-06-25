#include <Arduino.h>
#include "Motor/Motor.h"
#include "Joystick/Joystick.h"

#define LINK0 80
#define LINK1 80

#define MINPOSRADIUS 5

class Robot {
    private:
        bool enabled;
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
        void move2Target();

        float getXTarget();
        float getYTarget();
};
