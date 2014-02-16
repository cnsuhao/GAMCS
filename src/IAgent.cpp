/**********************************************************************

 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdio.h>
#include "IAgent.h"
#include "Debug.h"

IAgent::IAgent() :
        id(0), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0)
{
}

IAgent::IAgent(int i) :
        id(i), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0)
{
}

IAgent::IAgent(int i, float dr, float th) :
        id(i), discount_rate(dr), threshold(th), degree_of_curiosity(0.0)
{
    // check validity
    if (discount_rate >= 1.0 || discount_rate < 0)    // discount rate range [0, 1)
        ERROR(
                "Agent - discount rate must be bigger than 0 and smaller than 1.0!\n");

    if (threshold < 0)
    ERROR("Agent - threshold must be bigger than 0!\n");
}

IAgent::~IAgent()
{
}

/** \brief Restrict capacity of an agent.
 *  Comply with maximun payoff rule.
 * \param st state identity
 * \param acts all possible actions of st
 * \return action distribution after appling maximun payoff restrict
 *
 */
OSpace IAgent::Restrict(IAgent::State st,
        OSpace &acts)
{
    return MaxPayoffRule(st, acts);
}

/** \brief Update inner states.
 *
 */

void IAgent::Update(float oripayoff)
{
    UpdateMemory(oripayoff);    // update memory
    TSGIOM::Update();
    return;
}

