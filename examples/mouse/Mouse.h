#ifndef MOUSE_H_
#define MOUSE_H_
#include <string>
#include "Avatar.h"

using namespace gimcs;

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
        OSpace ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // MOUSE_H_
