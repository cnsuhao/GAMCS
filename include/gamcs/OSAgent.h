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
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------

#ifndef OSAGENT_H_
#define OSAGENT_H_

#include "gamcs/Agent.h"
#include "gamcs/Storage.h"

namespace gamcs
{

/**
 * @brief The Open Storage Agent is a kind of agent with storage interfaces that support manipulating the memory directly.
 */
class OSAgent: public Agent, public Storage
{
	public:
		/**
		 * @brief The default constructor.
		 *
		 * @param [in] id the agent id
		 * @param [in] discount_rate the discount rate
		 * @param [in] threshold the threshold
		 */
		OSAgent(int id = 0, float discount_rate = 0.9, float threshold = 0.01) :
				Agent(id, discount_rate, threshold)
		{
		}

		/**
		 * @brief The default destructor.
		 */
		virtual ~OSAgent()
		{
		}

		/**
		 * @brief Update payoffs starting from a specified state.
		 *
		 * @param [in] state the state where to start from
		 */
		virtual void updatePayoff(State state) = 0;
};

}    // namespace gamcs

#endif /* OSAGENT_H_ */
