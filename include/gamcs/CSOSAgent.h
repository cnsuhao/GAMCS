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
 * Computer Simulation of OSAgent.
 */
class CSOSAgent: public OSAgent
{
    public:
        typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash map from state value to state point */

        CSOSAgent(int id = 0, float discount_rate = 0.9,
                float threshold = 0.01);
        ~CSOSAgent();

        int open(Flag flag);    // connect to me
        void close();    // close connection to me

        State_Info_Header *getStateInfo(State state) const; /**< implementing GetStateInfo function */
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

        // iterate
        State firstState() const;
        State nextState() const;
        bool hasState(State state) const;

        void loadMemoryFromStorage(Storage *specific_storage); /**< load memory from storage */
        void dumpMemoryToStorage(Storage *specific_storage) const; /**< dump memory to storage */

    private:
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */

        struct cs_State *head; /**< memory head*/
        StatesMap states_map; /**< hash map from state values to state point */
        mutable struct cs_State *cur_mst; /**< state  for current state */
        mutable struct cs_State *current_st_index; /**< current state point used by iterator */

        std::deque<cs_State *> update_queue;
        std::unordered_set<cs_State *> visited_states;

        float prob(const struct cs_EnvAction *env_action,
                const struct cs_Action *action) const; /**< probability of a exact */
        OSpace bestActions(const struct cs_State *state,
                OSpace &available_actions) const; /**< find the best action of a state */
        OSpace maxPayoffRule(Agent::State state,
                OSpace &available_actions) const; /**< implementing maximum payoff rule */
        void updateStatePayoff(struct cs_State *state); /**< update state payoff backward recursively */
        void updateMemory(float original_payoff); /**< implementing updateMemory of Agent */

        void loadState(Storage *storage, Agent::State state); /**< load a state from storage to memory */

        void linkStates(struct cs_State *state, Agent::EnvAction env_action,
                Agent::Action action, struct cs_State *following_state); /**< link two states in memory with specific exact and action */

        struct cs_State *newState(Agent::State state); /**< create a new state  in memory */
        struct cs_Action *newAct(Agent::Action action, struct cs_State *state);
        struct cs_EnvAction *newEat(Agent::EnvAction env_action,
                struct cs_State *state, struct cs_Action *action);
        struct cs_BackwardLink *newBlk(struct cs_State *previous_state,
                struct cs_State *state);

        struct cs_State *searchState(Agent::State state) const; /**< search state in memory by its identity */
        struct cs_Action *searchAct(Agent::Action action,
                const struct cs_State *state) const; /**< find the Agent::Action  address according to identity */
        struct cs_EnvAction *searchEat(Agent::EnvAction env_action,
                struct cs_State *state, const struct cs_Action *action) const;
        struct cs_BackwardLink *searchBlk(struct cs_State *previous_state,
                const struct cs_State *state) const;

        void _deleteState(struct cs_State *state); /**< delete state from memory network */
        void deleteAct(Agent::Action action, struct cs_State *state);
        void deleteEat(Agent::EnvAction env_action,
                const struct cs_State *state, struct cs_Action *action);
        void deleteBlk(struct cs_State *previous_state, struct cs_State *state);

        void freeState(struct cs_State *state); /**< free a state  */
        void freeAct(struct cs_Action *action);
        void freeEat(struct cs_EnvAction *env_action);
        void freeBlk(struct cs_BackwardLink *backward_link);
        void freeMemory(); /**< free all space of memory in computer memory*/

        void buildStateFromHeader(
                const struct State_Info_Header *state_information_header,
                struct cs_State *state);

        float calStatePayoff(const struct cs_State *state) const; /**< calculate payoff of a state */
        float calActPayoff(Agent::Action action,
                const struct cs_State *state) const; /**< calculate payoff of an Agent::Action */
        float _calActPayoff(const struct cs_Action *action) const;
};

/** practical state structure used to represent a state */
struct cs_State
{
        Agent::State st; /**< state value */
        float payoff; /**< state payoff */
        float original_payoff; /**< original payoff of state */
        unsigned long count; /**< state count */
        struct cs_Action *actlist;
        struct cs_BackwardLink *blist; /**< backward links */

        struct cs_State *prev;
        struct cs_State *next;
};

/** practical action structure used to represent an action */
struct cs_Action
{
        Agent::Action act; /**< action value */
        struct cs_EnvAction *ealist;

        struct cs_Action *next;
};

/** practical structure to represent an environment action and the link to the following state */
struct cs_EnvAction
{
        Agent::EnvAction eat;
        unsigned long count; /**< eact count */
        struct cs_State *nstate; /**< next state */

        struct cs_EnvAction *next;
};

/** structure to connect a state with its previous states */
struct cs_BackwardLink
{
        struct cs_State *pstate; /**< previous state */
        struct cs_BackwardLink *next;
};

}    // namespace gamcs
#endif // CSOSAGENT_H_
