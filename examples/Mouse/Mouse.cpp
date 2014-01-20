/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Mouse.h"

Mouse1::Mouse1(int i):Avatar(i)
{
    position = 5;
}

Mouse1::~Mouse1()
{
    //dtor
}

Agent::State Mouse1::GetCurrentState()
{
    return position;
}

void Mouse1::DoAction(Agent::Action act)
{
    position += act;

    if (position > 20)
        position = 20;
    if (position < 1)
        position = 1;
    return;
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */

std::vector<Agent::Action> Mouse1::ActionCandidates(Agent::State st)
{
//    UNUSED(st);
    st = st;
    std::vector<Agent::Action> acts;
    acts.clear();
    acts.push_back(1);
    acts.push_back(-1);

    return acts;
}

float Mouse1::OriginalPayoff(Agent::State st)
{
    if (st == 15)
        return 1;
    else
        return 0;
}
