/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "R1Agent.h"

R1Agent::R1Agent(int n, int m, float dr, float th):MyAgent(n, m, dr, th)
{
}

R1Agent::~R1Agent()
{
}

float R1Agent::OriginalPayoff(State st)
{
    if (st == 196)
        return 1;
    else
        return 0;
}

State R1Agent::ActionEffect(State st, Action act)
{
    if (act == 1)
        return st-1;
    else // act == 2
        return st+1;
}
