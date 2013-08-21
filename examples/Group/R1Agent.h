#ifndef R1AGENT_H
#define R1AGENT_H

#include "../../MyAgent.h"

class R1Agent : public MyAgent
{
public:
    R1Agent(int, int, float, float, string);
    virtual ~R1Agent();

protected:
//    virtual vector<Action> OutList(State);
    virtual float OriginalPayoff(State);
};

#endif // R1AGENT_H
