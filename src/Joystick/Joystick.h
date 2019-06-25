#include <Arduino.h>

#define DEADZONE 64

class Joystick {
    private:
        char xPort;
        char yPort;

        short centerX, centerY;

        short rawX, rawY;

        short x, y;
    public:
        Joystick(char, char);
        void update();
        short getRawX();
        short getRawY();
        short getX();
        short getY();
};
