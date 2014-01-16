/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Agent.h"

Agent::Agent() : discount_rate(0.8), threshold(0.01)
{
}

Agent::Agent(float dr, float th) : discount_rate(dr), threshold(th)
{
}

Agent::~Agent()
{
}

/** \brief Restrict capacity of an agent.
 *  Comply with maximun payoff rule.
 * \param st state identity
 * \param acts all possible actions of st
 * \return action distribution after appling maximun payoff restrict
 *
 */
vector<Action> Agent::Restrict(State st, vector<Action> acts)
{
    return MaxPayoffRule(st, acts);
}

/** \brief Update inner states.
 *
 */

void Agent::Update(float oripayoff, State expst)
{
    UpdateMemory(oripayoff, expst); // update memory beginning from expst
    PFTGIOM::Update();
    return;
}
