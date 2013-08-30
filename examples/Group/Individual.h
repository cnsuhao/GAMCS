#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <signal.h>
#include "../../Avatar.h"

class Individual : public Avatar
{
    public:
        Individual(int i);
        virtual ~Individual();

    protected:
        void Run();
    private:
        State position;

        State GetCurrentState();
        void DoAction(Action);
        vector<Action> ActionList(State);
        State ExpectedState();
        float OriginalPayoff(State);
};

#endif // INDIVIDUAL_H
