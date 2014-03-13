#ifndef MONOMER_H
#define MONOMER_H
#include <string>
#include "gimcs/Avatar.h"

using namespace gimcs;

class Monomer: public Avatar
{
    public:
        /** Default constructor */
        Monomer(std::string);
        /** Default destructor */
        ~Monomer();

    private:
        Agent::State position;

        Agent::State percieveState();
        void performAction(Agent::Action);
        OSpace actionCandidates(Agent::State);
        float originalPayoff(Agent::State);
        void sendStateInfo(Agent::State)
        {
            // no social communication supported
        }
        void recvStateInfo()
        {
            // no social communication supported
        }
};

#endif
