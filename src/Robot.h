#include "Motor/Motor.h"
#include "Joystick/Joystick.h"

class Robot {
    private:
        long realX;
        long realY;
    public:
        Robot(char[], char, char[], char, char[], char);
        Motor *motor0;
        Motor *motor1;
        Joystick *js;
};
