#include <Arduino.h>

#define DEADZONE 20
#define CENTER 512

class Joystick {
    private:
        char xPort;
        char yPort;
        char ePort;

        long x, y;
        bool e;
    public:
        Joystick(char, char, char);
        void update();
        short getX();
        short getY();
        bool enabled();
};
