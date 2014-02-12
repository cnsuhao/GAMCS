#ifndef MONOMER_H
#define MONOMER_H
#include <vector>
#include <string>
#include "Avatar.h"

class Monomer: public Avatar
{
    public:
        /** Default constructor */
        Monomer(std::string);
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

#endif
