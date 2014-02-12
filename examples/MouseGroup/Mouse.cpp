/**********************************************************************
 *	@File:
 *	@Created: 2013-8-21
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include "Mouse.h"

Mouse::Mouse(std::string n) :
        CSThreadAvatar(n), count(0)
{
    position = 5;
}

Mouse::~Mouse()
{

}

Agent::State Mouse::GetCurrentState()
{
    return position;
}

void Mouse::PerformAction(Agent::Action act)
{
    position += act;

    if (position > 15) position = 15;
    if (position < 1) position = 1;
    return;
}

OutList Mouse::ActionCandidates(Agent::State st)
{
    if (count < 1000)
    {
        OutList acts;
        acts.clear();
        if (st == 1)    // position 1
        {
            acts.add(1);
            count++;
            return acts;
        }

        acts.add(1);
        acts.add(-1);
        count++;
        return acts;
    }
    else
        return OutList();    // return an empty list
}

float Mouse::OriginalPayoff(Agent::State st)
{
    if (st == 9)
        return 1;
    else if (st == 13)
        return 2;
    else
        return 0;
}
