#include <Arduino.h>

#define CCW 0
#define CW 1

#define STEPS_PER_REV 2048
#define MAX_FREQ 128

class Motor {
    private:
        // Pin settings
        char pins[4];
        char endstop;

        // State variables
        char state;
        short pos;
        short destPos;
        unsigned long lastStepTime;

        // Dynamically configurable variables
        unsigned long period;
        unsigned short frequency;
        bool dir;
        bool stopped;
        void step(bool);
    public:
        Motor(char[], char);

        bool zero();

        void goTo(short, unsigned short);

        bool checkAndStep();
        void move2Dest();
        void blockingStep();

        void setFrequency(unsigned short);
        unsigned short getFrequency();

        void setDirection(bool);
        bool getDirection();

        void setPos(short);
        short getPos();

        void setDestPos(short);
        short getDestPos();

        bool endstopHit();
};
