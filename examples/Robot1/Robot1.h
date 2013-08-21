#ifndef ROBOT1_H
#define ROBOT1_H
#include "R1Agent.h"

class Robot1
{
    public:
        /** Default constructor */
        Robot1():ra(20,2,0.8,0.01,"mem_file"), position(2){}
        /** Default destructor */
        virtual ~Robot1();

        void Run();
    protected:
    private:
        R1Agent ra;
        State position;

        State GetCurrentState();
        void DoAction(Action);
};

#endif // ROBOT1_H
