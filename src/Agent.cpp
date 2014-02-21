// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <stdio.h>
#include "gimcs/Agent.h"
#include "gimcs/Debug.h"

namespace gimcs
{

Agent::Agent() :
        id(0), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0)
{
}

Agent::Agent(int i) :
        id(i), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0)
{
}

Agent::Agent(int i, float dr, float th) :
        id(i), discount_rate(dr), threshold(th), degree_of_curiosity(0.0)
{
    // check validity
    if (discount_rate >= 1.0 || discount_rate < 0)    // discount rate range [0, 1)
        ERROR(
                "Agent - discount rate must be bigger than 0 and smaller than 1.0!\n");

    if (threshold < 0)
    ERROR("Agent - threshold must be bigger than 0!\n");
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
OSpace Agent::Restrict(Agent::State st, OSpace &acts) const
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

}    // namespace gimcs
