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
        IAgent::State position;
        int count;

        IAgent::State GetCurrentState();
        void PerformAction(IAgent::Action);
        OSpace ActionCandidates(IAgent::State);
        float OriginalPayoff(IAgent::State);
};

#endif // ROBOT1_H
