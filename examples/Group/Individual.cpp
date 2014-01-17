/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Individual.h"

Individual::Individual(int i):Avatar(i)
{
    position = 5;
}

Individual::~Individual()
{
    //dtor
    dbgmoreprt("~Individual\n");
}

State Individual::GetCurrentState()
{
    return position;
}

void Individual::DoAction(Action act)
{
    position += act;

    if (position > 15)
        position = 15;
    if (position < 1)
        position = 1;
    return;
}

vector<Action> Individual::ActionList(State st)
{
    UNUSED(st);
    vector<Action> acts;
    acts.clear();
    acts.push_back(1);
    acts.push_back(-1);

    return acts;
}

float Individual::OriginalPayoff(State st)
{
    if (st == 9)
        return 1;
    else if (st == 13)
        return 2;
    else
        return 0;
}
