#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <signal.h>
#include "R1Agent.h"
#include "../../Entity.h"

class Individual : public Entity
{
    public:
        Individual(int i);
        virtual ~Individual();

    protected:
        void Run();
    private:
        State position;

        State GetCurrentState();
        void DoAction(Action);
};

#endif // INDIVIDUAL_H
