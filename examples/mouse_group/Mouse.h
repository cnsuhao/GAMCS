#ifndef MOUSE_H
#define MOUSE_H
#include <string>
#include "CSThreadAvatar.h"

class Mouse: public CSThreadAvatar
{
    public:
        Mouse(std::string);
        ~Mouse();

    private:
        Agent::State position;
        int count;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        OutList ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // MOUSE_H
