/**********************************************************************
 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include "Agent.h"

Agent::Agent() :
        discount_rate(0.8), threshold(0.01), unseen_state_payoff(0.0), unseen_action_payoff(
                0.0), unseen_eaction_maxpayoff(0.0)
{
}

Agent::Agent(float dr, float th) :
        discount_rate(dr), threshold(th), unseen_state_payoff(0.0), unseen_action_payoff(
                0.0), unseen_eaction_maxpayoff(0.0)
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
std::vector<Agent::Action> Agent::Restrict(Agent::State st,
        const std::vector<Agent::Action> &acts)
{
    return MaxPayoffRule(st, acts);
}

/** \brief Update inner states.
 *
 */

void Agent::Update(float oripayoff)
{
    UpdateMemory(oripayoff);    // update memory
    TSGIOM::Update();
    return;
}
