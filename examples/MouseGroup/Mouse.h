#ifndef MOUSE_H
#define MOUSE_H
#include <vector>
#include "CSThreadAvatar.h"

class Mouse: public CSThreadAvatar
{
    public:
        Mouse(int i);
        ~Mouse();

    private:
        Agent::State position;

        Agent::State GetCurrentState();
        void DoAction(Agent::Action);
        std::vector<Agent::Action> ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // MOUSE_H
