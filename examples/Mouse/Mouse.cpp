/**********************************************************************
 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include "Mouse.h"

Mouse::Mouse(int i) :
        Avatar(i)
{
    position = 3;
}

Mouse::~Mouse()
{
    //dtor
}

Agent::State Mouse::GetCurrentState()
{
    return position;
}

void Mouse::DoAction(Agent::Action act)
{
    position += act;

    if (position > 8) position = 8;
    if (position < 1) position = 1;
    return;
}

/** \brief Get all outputs of each possible input.
 * By default, for a "I:N/O:M" it will return outputs with values from 1 to M for each input.
 *
 * \param in input identity
 * \return all possible outputs for the input
 *
 */

std::vector<Agent::Action> Mouse::ActionCandidates(Agent::State st)
{
//    UNUSED(st);
    static int count;
//    UNUSED(st);
    if (count < 50)
    {
        std::vector<Agent::Action> acts;
        acts.clear();
        acts.push_back(1);
        acts.push_back(-1);
        count++;
        return acts;
    }
    else
        return std::vector<Agent::Action>();    // return an empty list
}

float Mouse::OriginalPayoff(Agent::State st)
{
    if (st == 6)
        return 1;
    else
        return 0;
}
