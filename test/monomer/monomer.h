#ifndef MONOMER_H
#define MONOMER_H
#include <string>
#include "Incarnation.h"

class Monomer: public Incarnation
{
    public:
        /** Default constructor */
        Monomer(std::string);
        /** Default destructor */
        ~Monomer();

    private:
        IAgent::State position;

        IAgent::State GetCurrentState();
        void PerformAction(IAgent::Action);
        OSpace ActionCandidates(IAgent::State);
        float OriginalPayoff(IAgent::State);
        void SendStateInfo(IAgent::State)
        {
            // no social communication supported
        }
        void RecvStateInfo()
        {
            // no social communication supported
        }
};

#endif
