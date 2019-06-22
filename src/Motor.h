#define CCW 0
#define CW 1

#define STEPS_PER_REV 2048
#define MAX_FREQ 200

class Motor {
    private:
        char pins[4];
        char state;
        unsigned long lastStepTime;
        unsigned long period;
        bool dir;
        bool stopped;
    public:
        Motor(char[]);
        void step(bool dir);
        bool checkAndStep();
        void setFrequency(unsigned short);
        void setDirection(bool dir);
};
