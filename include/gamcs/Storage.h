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
 *  @brief Storage Interface
 */
class Storage
{
	public:
		/**
		 * @brief The default constructor.
		 */
		Storage()
		{
		}

		/**
		 * @brief The default destructor.
		 */
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

		/**
		 * @brief Open storage for read or write.
		 *
		 * @param [in] flag the open flag
		 * @return 0 if okay, otherwise a negative value
		 * @see close()
		 */
		virtual int open(Flag flag) = 0;
		/**
		 * @brief Close storage.
		 * @see open()
		 */
		virtual void close() = 0;
		/**
		 * @brief Get the information of a specified state value.
		 *
		 * @param [in] state the specified state
		 * @return address point of the state information
		 */
		virtual struct State_Info_Header *getStateInfo(
				Agent::State state) const = 0; /**< get the information of a specified state value */
		/**
		 * @brief Add a state to storage from the given information.
		 *
		 * @param [in] state_information_header the state information
		 */
		virtual void addStateInfo(
				const struct State_Info_Header *state_information_header) = 0; /**< add a state to storage from the given information */
		/**
		 * @brief Update a state in storage from the given information.
		 *
		 * @param [in] state_information_header the state information
		 */
		virtual void updateStateInfo(
				const struct State_Info_Header *state_information_header) = 0; /**< update a state in storage from the given information */
		/**
		 * @brief Delete a state from storage.
		 *
		 * @param [in] state the state to be deleted
		 */
		virtual void deleteState(Agent::State state) = 0; /**< delete a state from storage */

		/**
		 * @brief Get the memory information from storage.
		 *
		 * @return address point of the memory information
		 */
		virtual struct Memory_Info *getMemoryInfo() const = 0; /**< get the memory information from storage */
		/**
		 * @brief Add the memory information to storage.
		 *
		 * @param [in] memory_information_header the memory information
		 */
		virtual void addMemoryInfo(
				const struct Memory_Info *memory_information_header) = 0; /**< add the memory information to storage */
		/**
		 * @brief Update the memory information in storage.
		 *
		 * @param [in] memory_information_header the memory information
		 */
		virtual void updateMemoryInfo(
				const struct Memory_Info *memory_information_header) = 0; /**< update the memory information in storage */
		/**
		 * @brief Get the memory information from storage.
		 *
		 * @return address point of the memory information
		 */
		virtual std::string getMemoryName() const = 0; /**< get the memory name */

		/* iterate all states */
		/**
		 * @brief Get the first state in storage.
		 *
		 * @return the first state
		 */
		virtual Agent::State firstState() const = 0; /**< get the first state in storage */
		/**
		 * @brief Get the next state in storage
		 *
		 * @return the next state
		 */
		virtual Agent::State nextState() const = 0; /**< get the next state in storage */
		/**
		 * @brief Check if a state exists in storage.
		 *
		 * @param [in] state the requested state
		 * @return true|false
		 */
		virtual bool hasState(Agent::State state) const = 0; /**< check if a state exists in storage */

};

}    // namespace gamcs
#endif /* STORAGE_H_ */
