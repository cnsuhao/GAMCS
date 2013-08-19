/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Agent.h"

Agent::Agent(int n, int m):PFTGIOM(n, m)
{
    //ctor
    discount_rate = 0.8;
}

Agent::Agent(int n, int m, float dr):PFTGIOM(n, m)
{
    discount_rate = dr;
}

Agent::~Agent()
{
    //dtor
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

/** \brief Get original payoff of each state.
 *  Return 1 for every state.
 * \param st state identity
 * \return original payoff of st
 *
 */

float Agent::OriginalPayoff(State st)
{
    return 1;
}
