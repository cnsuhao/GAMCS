#ifndef ROBOT1_H
#define ROBOT1_H
#include <string>
#include "Incarnation.h"

class Mouse: public Incarnation
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

#endif // ROBOT1_H
