#ifndef MONOMER_H
#define MONOMER_H
#include <string>
#include "gamcs/Avatar.h"

using namespace gamcs;

class Monomer: public Avatar
{
    public:
        /** Default constructor */
        Monomer();
        /** Default destructor */
        ~Monomer();

    private:
        Agent::State position;

        Agent::State perceiveState();
        void performAction(Agent::Action);
        OSpace availableActions(Agent::State);
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
