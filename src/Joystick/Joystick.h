#include <Arduino.h>

#define DEADZONE 40

class Joystick {
    private:
        char xPort;
        char yPort;
        char ePort;

        short centerX, centerY;

        short rawX, rawY;

        short x, y;
        bool e;
    public:
        Joystick(char, char, char);
        void update();
        short getRawX();
        short getRawY();
        short getX();
        short getY();
        bool enabled();
};
