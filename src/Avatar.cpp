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

#include <stdlib.h>
#include <sys/timeb.h>
#include "gamcs/Avatar.h"
#include "gamcs/debug.h"
#include "gamcs/platforms.h"

namespace gamcs
{

Avatar::Avatar(int i) :
		id(i), ava_loop_count(0), myagent(NULL)
{
}

Avatar::~Avatar()
{
}

/**
 * @brief Step avatar for one circle.
 *
 * Note: if there are no more actions for an avatar to perform, the step will return -1 which means the end is reached.
 * @return 0 on success, -1 if reach the end
 */
int Avatar::step()
{
	++ava_loop_count;    // increase count

	/* Perceive state */
	Agent::State cs = percieveState();    // get current state
	dbgmoreprt("Launch():", "Avatar %d, State: %" ST_FMT "\n", id, cs);

	/* Process */
	OSpace acts = availableActions(cs);    // get all action candidates of a state

	Agent::Action act = myagent->process(cs, acts);    // choose an action from candidates
	// check validation
	if (act == Agent::INVALID_ACTION)    // no valid actions available, reach a dead end, quit. !!!: be sure to check this before update stage
		return -1;

	/* update memory */
	myagent->update(originalPayoff(cs));    // agent update inner states

	/* Perform action */
	performAction(act);    // otherwise, perform the action

	return 0;
}

/**
 * @brief Step avatar in loop
 * @param sps steps/loops per second, < 0 if not control
 */
void Avatar::loop(int sps)
{
	// check if agent is connected
	if (myagent == NULL)
		ERROR("launch(): Avatar is not connected to any agent!\n");

	unsigned long control_step_time = 0; /**< delta time in millisecond requested bewteen two steps */
	if (sps > 0)
		control_step_time = 1000 / sps;    // (1 / sps) * 1000

	unsigned long start_time = 0;

	while (true)
	{
		dbgmoreprt("Enter Launch Loop ", "------------------------------ count == %ld\n", ava_loop_count);
		if (sps > 0)
			start_time = getCurrentTime();

		int re = step();
		if (re == -1)    // break if no actions available
			break;

		// handle time related job
		if (sps > 0)    // no control when sps <= 0
		{
			unsigned long consumed_time = getCurrentTime() - start_time;
			long time_remaining = control_step_time - consumed_time;
			if (time_remaining > 0)    // remaining time
			{
				dbgmoreprt("",
						"You got %ld milliseconds remaining to do other things.\n",
						time_remaining);
				// do some useful things here if you don't want to sleep
				pi_msleep(time_remaining);
			}
			else
			{
				WARNNING(
						"time is not enough to run a step, %ldms in lack, try to decrease the sps!\n",
						-time_remaining);
			}
		}
	}
	// quit
	dbgmoreprt("Exit Launch Loop", "----------------------------------------------------------- %s Exit!\n", name.c_str());
	return;
}

/**
 * @brief Connect avatar to an agent.
 * @param agt the agent to be connected
 */
void Avatar::connectAgent(Agent *agt)
{
	myagent = agt;
}

/**
 * @brief Get the original payoff of a state.
 *
 * Return 1 for every state by default which means as long as the avatar survives in the next state, the state has payoff as 1.
 * @param st the state
 * @return original payoff of the state
 *
 */
float Avatar::originalPayoff(Agent::State st)
{
	UNUSED(st);
	return 1;    // original payoff of states is 1.0 by default
}

/**
 * @brief Get current time in milliseconds
 * @return the current time as milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
 */
unsigned long Avatar::getCurrentTime()
{
	struct timeb tb;
	ftime(&tb);
	return 1000 * tb.time + tb.millitm;
}

}    // namespace gamcs
