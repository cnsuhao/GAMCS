#ifndef R1AGENT_H
#define R1AGENT_H

#include "../../MyAgent.h"

class R1Agent : public MyAgent
{
public:
    R1Agent(int, int, float, float, string);
    virtual ~R1Agent();

    void DoNothing();
protected:
//    virtual vector<Action> OutList(State);
    virtual float OriginalPayoff(State);
    virtual State ActionEffect(State, Action);

};

#endif // R1AGENT_H
