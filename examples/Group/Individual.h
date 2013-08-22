#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <signal.h>
#include "R1Agent.h"
#include "../../SimGroup.h"

class Individual
{
    public:
        Individual(int i, SimGroup *gp);
        virtual ~Individual();

        int ThreadRun();
        void SetFreq(int);
    protected:
        void Run();
    private:
        int id;
        int freq;
        R1Agent *ra;            // must be point, initialize it with NEW
        SimGroup *grp;
        State position;

        void SendStateInfo(State);
        void RecvStateInfo();

        State GetCurrentState();
        void DoAction(Action);

        static void* hook(void* args) {
            reinterpret_cast<Individual*>(args)->Run();
            return NULL;
        }
};

#endif // INDIVIDUAL_H
