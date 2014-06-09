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

const Agent::State Agent::INVALID_STATE = INVALID_INPUT; /**< the same as input, since it's just an alias to input */
const Agent::Action Agent::INVALID_ACTION = INVALID_OUTPUT; /**< the same as output, since it's just an alias to output */
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

/**
 * @brief Set the learning mode of an agent.
 * @param mode the learning mode
 */
void Agent::setMode(Mode mode)
{
	learning_mode = mode;
}

/**
 * @brief The constraining capacity of an agent.
 * Use the Maximum Payoff Rule to do the constraining.
 * @param st the state value
 * @param acts the action space of current state
 * @return the sub action space after constraining
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

/**
 * @brief Update the inner data of an agent.
 */
void Agent::update(float oripayoff)
{
	updateMemory(oripayoff);    // update memory
	TSGIOM::update();    // as a TSGIOM, invoke the basic update function
	return;
}

}    // namespace gamcs
