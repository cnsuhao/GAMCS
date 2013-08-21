/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "R1Agent.h"

R1Agent::R1Agent(int n, int m, float dr, float th, string mf):MyAgent(n, m, dr, th, mf)
{
}

R1Agent::~R1Agent()
{
}

float R1Agent::OriginalPayoff(State st)
{
    if (st == 9)
        return 1;
    else
        return 0;
}
