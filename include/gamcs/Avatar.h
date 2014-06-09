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
//
// Created on: Oct 19, 2013
//
// -----------------------------------------------------------------------------

#ifndef AVATAR_H_
#define AVATAR_H_
#include <string>
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 * Avatar is an agent embodied in flesh.
 *
 * Each avatar has to be connected to an agent before activated, which is the soul and mind of that avatar.
 */
class Avatar
{
	public:
		Avatar(int id = 0);
		virtual ~Avatar();

		int step();
		void loop(int steps_per_second = -1);
		void connectAgent(Agent *agent);

	protected:
		int id; /**< avatar id */
		unsigned long ava_loop_count; /**< loop count */

		Agent *myagent; /**< the connected agent */

		virtual Agent::State percieveState() = 0; /**< perceive the environment and get the current state */
		virtual void performAction(Agent::Action action) = 0; /**< perform an specified action */
		virtual OSpace availableActions(Agent::State state) = 0; /**< get the action space of a specified state */
		virtual float originalPayoff(Agent::State state);

	private:
		unsigned long getCurrentTime();
};

}    // namespace gamcs
#endif // AVATAR_H_
