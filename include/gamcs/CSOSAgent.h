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

#ifndef CSOSAGENT_H_
#define CSOSAGENT_H_
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include "gamcs/OSAgent.h"

namespace gamcs
{

/**
 * @brief CSOSAgent is an implementation of OSAgent using computer.
 */
class CSOSAgent: public OSAgent
{
	public:
		typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash mapping from state value to the address point stored that state */

		CSOSAgent(int id = 0, float discount_rate = 0.9,
				float accuracy = 0.01);
		~CSOSAgent();

		int open(Flag flag);
		void close();

		State_Info_Header *getStateInfo(State state) const;
		void addStateInfo(
				const struct State_Info_Header * state_information_header);
		void updateStateInfo(
				const struct State_Info_Header *state_information_header);
		DEPRECATED("Be careful when used with a storage, currently states will not be deleted from storage, and this will lead to the storage inconsistent!\n")
		void deleteState(State state);
		void updatePayoff(State state);

		struct Memory_Info *getMemoryInfo() const;
		void addMemoryInfo(const struct Memory_Info *memory_info_header);
		void updateMemoryInfo(const struct Memory_Info *memory_info_header);
		std::string getMemoryName() const;

		// iterator
		State firstState() const;
		State nextState() const;
		bool hasState(State state) const;

		typedef void (*progbar_callback) (unsigned long index, unsigned total, char *label);
		void loadMemoryFromStorage(Storage *specific_storage, progbar_callback progbar = NULL);
		void dumpMemoryToStorage(Storage *specific_storage, progbar_callback progbar = NULL) const;

	private:
		unsigned long state_num; /**< total number of states in memory */
		unsigned long lk_num; /**< total number of links between states in memory */

		struct cs_State *head; /**< the starting address of agent memory */
		StatesMap states_map; /**< the hash map from state values to the address point stored that state */
		mutable struct cs_State *cur_mst; /**< state structure for current state */
		mutable struct cs_State *current_st_index; /**< current state address point used by iterator */

		std::deque<cs_State *> update_queue; /**< the states to be updated */
		std::unordered_set<cs_State *> visited_states; /**< the states that has been updated in an update circle */

		float prob(const struct cs_EnvAction *env_action,
				const struct cs_Action *action) const;
		OSpace bestActions(const struct cs_State *state,
				OSpace &available_actions) const;
		OSpace maxPayoffRule(Agent::State state,
				OSpace &available_actions) const;
		void updateStatePayoff(struct cs_State *state);
		void updateMemory(float original_payoff);

		void loadState(Storage *storage, Agent::State state);

		void linkStates(struct cs_State *state, Agent::EnvAction env_action,
				Agent::Action action, struct cs_State *following_state);

		struct cs_State *newState(Agent::State state);
		struct cs_Action *newAct(Agent::Action action, struct cs_State *state);
		struct cs_EnvAction *newEat(Agent::EnvAction env_action,
				struct cs_State *state, struct cs_Action *action);
		struct cs_BackwardLink *newBlk(struct cs_State *previous_state,
				struct cs_State *state);

		struct cs_State *searchState(Agent::State state) const;
		struct cs_Action *searchAct(Agent::Action action,
				const struct cs_State *state) const;
		struct cs_EnvAction *searchEat(Agent::EnvAction env_action,
				struct cs_State *state, const struct cs_Action *action) const;
		struct cs_BackwardLink *searchBlk(struct cs_State *previous_state,
				const struct cs_State *state) const;

		void _deleteState(struct cs_State *state);
		void deleteAct(Agent::Action action, struct cs_State *state);
		void deleteEat(Agent::EnvAction env_action,
				const struct cs_State *state, struct cs_Action *action);
		void deleteBlk(struct cs_State *previous_state, struct cs_State *state);

		void freeState(struct cs_State *state);
		void freeAct(struct cs_Action *action);
		void freeEat(struct cs_EnvAction *env_action);
		void freeBlk(struct cs_BackwardLink *backward_link);
		void freeMemory();

		void buildStateFromHeader(
				const struct State_Info_Header *state_information_header,
				struct cs_State *state);

		float calStatePayoff(const struct cs_State *state) const;
		float calActPayoff(Agent::Action action,
				const struct cs_State *state) const;
		float _calActPayoff(const struct cs_Action *action) const;
		float trimPayoff(float payoff) const;
};

/**
 * @brief The structure used to represent a state in computer memory
 */
struct cs_State
{
		Agent::State st; /**< the state value */
		float payoff; /**< state payoff */
		float original_payoff; /**< original payoff of the state */
		unsigned long count; /**< experiencing count */
		struct cs_Action *actlist; /**< performed actions under this state */
		struct cs_BackwardLink *blist; /**< which states have this state as their following state */

		struct cs_State *prev; /**< the previous state */
		struct cs_State *next; /**< the next state */
};

/**
 * @brief The structure used to represent an action in computer memory
 */
struct cs_Action
{
		Agent::Action act; /**< the action value */
		struct cs_EnvAction *ealist; /**< observed environment actions under this action */

		struct cs_Action *next; /**< the next action */
};

/**
 * @brief The structure used to represent an environment action in computer memory
 */
struct cs_EnvAction
{
		Agent::EnvAction eat; /**< the action value */
		unsigned long count; /**< experiencing count */
		struct cs_State *nstate; /**< the following state of this action */

		struct cs_EnvAction *next; /**< next environment action */
};

/**
 * @brief The structure used to represent a link between a state and its up-streaming states
 */
struct cs_BackwardLink
{
		struct cs_State *pstate; /**< the up-streaming state */
		struct cs_BackwardLink *next; /**< next backward link */
};

}    // namespace gamcs
#endif // CSOSAGENT_H_
