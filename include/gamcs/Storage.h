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
// Created on: Jan 21, 2014
//
// -----------------------------------------------------------------------------

#ifndef STORAGE_H_
#define STORAGE_H_
#include <string>
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 *  Storage Interface
 */
class Storage
{
	public:
		Storage()
		{
		}

		virtual ~Storage()
		{
		}

		/**
		 * Flags for opening a storage
		 */
		enum Flag
		{
			O_READ = 0, /**< open to read */
			O_WRITE /**< open to write */
		};

		virtual int open(Flag flag) = 0; /**< open storage for read or write */
		virtual void close() = 0; /**< close storage */

		virtual struct State_Info_Header *getStateInfo(
				Agent::State state) const = 0; /**< get the information of a specified state value */
		virtual void addStateInfo(
				const struct State_Info_Header *state_information_header) = 0; /**< add a state to storage from the given information */
		virtual void updateStateInfo(
				const struct State_Info_Header *state_information_header) = 0; /**< update a state in storage from the given information */
		virtual void deleteState(Agent::State state) = 0; /**< delete a state from storage */

		virtual struct Memory_Info *getMemoryInfo() const = 0; /**< get the memory information from storage */
		virtual void addMemoryInfo(
				const struct Memory_Info *memory_information_header) = 0; /**< add the memory information to storage */
		virtual void updateMemoryInfo(
				const struct Memory_Info *memory_information_header) = 0; /**< update the memory information in storage */
		virtual std::string getMemoryName() const = 0; /**< get the memory name */

		/* iterate all states */
		virtual Agent::State firstState() const = 0; /**< get the first state in storage */
		virtual Agent::State nextState() const = 0; /**< get the next state in storage */
		virtual bool hasState(Agent::State state) const = 0; /**< check if a state exists in storage */

};

}    // namespace gamcs
#endif /* STORAGE_H_ */
