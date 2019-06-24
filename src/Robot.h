#include <Arduino.h>
#include "Motor/Motor.h"
#include "Joystick/Joystick.h"

#define LINK0 80
#define LINK1 80

#define JOY2SPD 10.0/512.0

class Robot {
    private:
        float xPos;
        float yPos;
        float xSpd;
        float ySpd;
    public:
        Robot(char[], char, char[], char, char[], char);
        Motor *motor0;
        Motor *motor1;
        Joystick *js;

        void home();
        void behavior0();
        void update();
};
