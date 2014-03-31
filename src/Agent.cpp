// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <stdio.h>
#include "gamcs/Agent.h"
#include "gamcs/debug.h"

namespace gamcs
{

Agent::Agent(int i, float dr, float th) :
        id(i), discount_rate(dr), threshold(th)
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

/** \brief constrain capacity of an agent.
 *  Comply with maximun payoff rule.
 * \param st state identity
 * \param acts all possible actions of st
 * \return action distribution after appling maximun payoff restrict
 *
 */
OSpace Agent::constrain(Agent::State st, OSpace &acts) const
{
    return maxPayoffRule(st, acts);
}

/** \brief update inner states.
 *
 */

void Agent::update(float oripayoff)
{
    updateMemory(oripayoff);    // update memory
    TSGIOM::update();
    return;
}

}    // namespace gamcs
