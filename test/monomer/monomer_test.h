#ifndef ROBOT1_H
#define ROBOT1_H
#include <vector>
#include "Avatar.h"

class Monomer: public Avatar
{
    public:
        /** Default constructor */
        Monomer(int);
        /** Default destructor */
        ~Monomer();

    private:
        Agent::State position;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        std::vector<Agent::Action> ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
        void SendStateInfo(Agent::State)
        {
            // no social communication supported
        }
        void RecvStateInfo()
        {
            // no social communication supported
        }
};

#endif // ROBOT1_H
