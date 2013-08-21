#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <pthread.h>
#include <signal.h>
#include "R1Agent.h"
#include "../../SimGroup.h"

class Individual
{
    public:
        Individual(int i);
        virtual ~Individual();

        int ThreadRun();
        void SetFreq(int);
        void JoinGroup(SimGroup);
    protected:
        void Run();
    private:
        int id;
        int freq;
        R1Agent ra;
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
