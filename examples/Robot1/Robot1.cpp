/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Robot1.h"

Robot1::Robot1(int i):Avatar(i)
{
    position = 5;
}

Robot1::~Robot1()
{
    //dtor
}

Agent::State Robot1::GetCurrentState()
{
    return position;
}

void Robot1::DoAction(Agent::Action act)
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

std::vector<Agent::Action> Robot1::ActionCandidates(Agent::State st)
{
//    UNUSED(st);
    st = st;
    std::vector<Agent::Action> acts;
    acts.clear();
    acts.push_back(1);
    acts.push_back(-1);

    return acts;
}

float Robot1::OriginalPayoff(Agent::State st)
{
    if (st == 15)
        return 1;
    else
        return 0;
}
