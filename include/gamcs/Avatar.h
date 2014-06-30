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
 * @brief Avatar is an agent embodied in flesh.
 *
 * Each avatar has to be connected to an agent before activated, which is the soul and mind of that avatar.
 */
class Avatar
{
	public:
		Avatar(int id = 0);
		virtual ~Avatar();

		int step();
		void pstep();
		void loop(int steps_per_second = -1);
		void connectAgent(Agent *agent);

	protected:
		int id; /**< avatar id */
		unsigned long ava_loop_count; /**< loop count */

		Agent *myagent; /**< the connected agent */

		/**
		 * @brief Perceive the environment and get the current state.
		 *
		 * In a typical situation, your avatar may have several sensors installed.
		 * Collect the data from these sensors, encapsulate them into a single integer(state) and return it.
		 * Make sure you encapsulate in the way that two different combinations of sensing data will have two different states.
		 *
		 * For example, if your avatar has 3 sensors, and each sensor has a data range as listed below:
		 *
		 * <table>
		 * <tr> <td>sensors</td> <td>ranges</td> <td>value</td> </tr>
		 * <tr> <td>s1</td> <td>[0, m1]</td> <td>v1</td> </tr>
		 * <tr> <td>s2</td> <td>[0, m2]</td> <td>v2</td> </tr>
		 * <tr> <td>s3</td> <td>[0, m3]</td> <td>v3</td> </tr>
		 * </table>
		 *
		 * You could do the encapsulating like this: \f$ state = v1 + v2 * m1 + v3 * m1 * m2 \f$.
		 *
		 * Be careful to not exceed the maximum value a state can represent (INT_BITS).
		 * @return the perceived state
		 */
		virtual Agent::State perceiveState() = 0;

		/**
		 * @brief Perform an specified action.
		 *
		 * All actions are represented by integers, but the representations are totally optional.
		 * The only limitation is that two different actions better not have the same integer.
		 *
		 * For example, you want to choose two integers to represent the two actions your toy car can perform, that is moving one step forward and moving one step backward.
		 *
		 * Either you can use integer 1 to represent the forward action and integer 2 for the backward action.
		 * Or you can use integer -1 for the forward action and integer 1 for the backward action.
		 *
		 * In fact, any two different singed integers not exceeding INT_BITS are valid.
		 * @param [in] action the action to be performed
		 */
		virtual void performAction(Agent::Action action) = 0;

		/**
		 * @brief Get the action space of a specified state.
		 *
		 * A state with an empty action space will be the dead-end state where your avatar will suspend.
		 * @param [in] state the specified state
		 * @return the action space of that state
		 */
		virtual OSpace availableActions(Agent::State state) = 0;
		virtual float originalPayoff(Agent::State state);

	private:
		unsigned long getCurrentTime();
};

}    // namespace gamcs
#endif // AVATAR_H_
