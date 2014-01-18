#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <signal.h>
#include <vector>
#include "Avatar.h"

class Individual : public Avatar
{
    public:
        Individual(int i);
        virtual ~Individual();

    protected:
        void Run();
    private:
        Agent::State position;

        Agent::State GetCurrentState();
        void DoAction(Agent::Action);
        std::vector<Agent::Action> ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // INDIVIDUAL_H
