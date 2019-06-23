#define CCW 0
#define CW 1

#define STEPS_PER_REV 2048
#define MAX_FREQ 200

class Motor {
    private:
        // Pin settings
        char pins[4];
        char endstop;

        // State variables
        char state;
        short pos;
        unsigned long lastStepTime;

        // Dynamically configurable variables
        unsigned long period;
        bool dir;
        bool stopped;
    public:
        Motor(char[], char);

        bool zero();

        void step(bool dir);
        bool checkAndStep();
        void blockingStep();

        void setFrequency(unsigned short);
        void setDirection(bool dir);
};
