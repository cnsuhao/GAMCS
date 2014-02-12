#ifndef ROBOT1_H
#define ROBOT1_H
#include <string>
#include "Avatar.h"

class Mouse: public Avatar
{
    public:
        /** Default constructor */
        Mouse(std::string);
        /** Default destructor */
        ~Mouse();

    private:
        Agent::State position;
        int count;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        OutList ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // ROBOT1_H
