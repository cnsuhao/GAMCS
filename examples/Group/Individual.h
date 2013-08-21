#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <pthread.h>
#include <signal.h>
#include "../../MyAgent.h"
#include "../../Entity.h"
#include "SimGroup.h"

class Individual : public Entity
{
    public:
        Individual(int);
        virtual ~Individual();

        void Run();
        void SetFreq(int);
    protected:
    private:
        int id;
        int freq;
        My

        void SignalHandler(int);
        void SendStateInfo();
        void RecvStateInfo();
};

#endif // INDIVIDUAL_H
