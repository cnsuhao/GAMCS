// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <cfloat>
#include "gamcs/Agent.h"
#include "gamcs/debug.h"

namespace gamcs
{

const Agent::State Agent::INVALID_STATE = INVALID_INPUT;
const Agent::Action Agent::INVALID_ACTION = INVALID_OUTPUT;
const float Agent::INVALID_PAYOFF = FLT_MAX; /**< use the maximum value to represent the invalid payoff */

Agent::Agent(int i, float dr, float th) :
        id(i), discount_rate(dr), threshold(th), learning_mode(ONLINE)
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

void Agent::setMode(Mode mode)
{
    learning_mode = mode;
}

/** \brief Constraint capacity of an agent.
 *  Comply with the maximum payoff rule.
 * \param st state value
 * \param acts all possible actions of st
 * \return action distribution after applying maximum payoff restrict
 *
 */
OSpace Agent::constrain(Agent::State st, OSpace &acts) const
{
    if (learning_mode == ONLINE)    // using maxPayoffRule in ONLINE mode
        return maxPayoffRule(st, acts);
    else if (learning_mode == EXPLORE)    // no constraint at all in EXPLORE mode
        return acts;
    else
    {
        ERROR("Unknown learning mode: %d!\n", learning_mode);
        return OSpace();
    }
}

/** \brief Update inner states.
 *
 */
void Agent::update(float oripayoff)
{
    updateMemory(oripayoff);    // update memory
    TSGIOM::update();
    return;
}

}    // namespace gamcs
