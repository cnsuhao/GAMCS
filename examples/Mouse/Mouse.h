#ifndef ROBOT1_H
#define ROBOT1_H
#include <signal.h>
#include "Avatar.h"

class Mouse1 : public Avatar
{
    public:
        /** Default constructor */
        Mouse1(int);
        /** Default destructor */
        virtual ~Mouse1();

    protected:
    private:
        Agent::State position;

        Agent::State GetCurrentState();
        void DoAction(Agent::Action);
        std::vector<Agent::Action> ActionCandidates(Agent::State);
        Agent::State ExpectedState();
        float OriginalPayoff(Agent::State);
};

#endif // ROBOT1_H
