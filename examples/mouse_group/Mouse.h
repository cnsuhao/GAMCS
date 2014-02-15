#ifndef MOUSE_H
#define MOUSE_H
#include <string>
#include "CSThreadIncarnation.h"

class Mouse: public CSThreadIncarnation
{
    public:
        Mouse(std::string);
        ~Mouse();

    private:
        IAgent::State position;
        int count;

        IAgent::State GetCurrentState();
        void PerformAction(IAgent::Action);
        OSpace ActionCandidates(IAgent::State);
        float OriginalPayoff(IAgent::State);
};

#endif // MOUSE_H
