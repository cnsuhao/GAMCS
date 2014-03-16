// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
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
#include <unordered_map>
#include "gamcs/OSAgent.h"

namespace gamcs
{

/**
 * Computer Simulation OSAgent, where computer was used to implement an agent.
 */
class CSOSAgent: public OSAgent
{
    public:
        typedef std::unordered_map<Agent::State, void *> StatesMap; /**< hash map from state value to state  */

        CSOSAgent();
        CSOSAgent(int);
        CSOSAgent(int, float, float);
        ~CSOSAgent();

        int connect();    // connect to agent memory
        void close();    // close connection to agent memory

        State_Info_Header *getStateInfo(State) const; /**< implementing GetStateInfo function */
        void addStateInfo(const struct State_Info_Header *);
        void updateStateInfo(const struct State_Info_Header *);
        void deleteState(State);
        void updatePayoff(State);

        void addMemoryInfo(const struct Memory_Info *);
        struct Memory_Info *getMemoryInfo() const;
        std::string getMemoryName() const;

        // iterate
        State firstState() const;
        State nextState() const;
        bool hasState(State) const;

        void loadMemoryFromStorage(Storage *); /**< load memory from storage */
        void dumpMemoryToStorage(Storage *) const; /**< dump memory to storage */

    private:
        unsigned long state_num; /**< total number of states in memory */
        unsigned long lk_num; /**< total number of links between states in memory */

        struct cs_State *head; /**< memory head*/
        StatesMap states_map; /**< hash map from state values to state  */
        mutable struct cs_State *cur_mst; /**< state  for current state */
        mutable struct cs_State *current_st_index; /**< current state point used by iterator */

        float prob(const struct cs_EnvAction*, const struct cs_Action *) const; /**< probability of a exact */
        OSpace bestActions(const struct cs_State *, OSpace&) const; /**< find the best action of a state */
        OSpace maxPayoffRule(Agent::State, OSpace &) const; /**< implementing maximun payoff rule */
        void updateStatePayoff(struct cs_State *); /**< update state payoff backward recursively */
        void updateMemory(float); /**< implementing updateMemory of Agent */

        void printProcess(unsigned long, unsigned long, char *) const;

        void loadState(Storage *, Agent::State); /**< load a state from storage to memory */

        void linkStates(struct cs_State *, Agent::EnvAction, Agent::Action,
                struct cs_State *); /**< link two states in memory with specfic exact and action */

        struct cs_State *newState(Agent::State); /**< create a new state  in memory */
        struct cs_Action *newAct(Agent::Action, struct cs_State *);
        struct cs_EnvAction *newEat(Agent::EnvAction, struct cs_State *,
                struct cs_Action *);
        struct cs_BackwardLink *newBlk(struct cs_State *, struct cs_State *);

        struct cs_State *searchState(Agent::State) const; /**< search state in memory by its identity */
        struct cs_Action *searchAct(Agent::Action,
                const struct cs_State *) const; /**< find the Agent::Action  address according to identity */
        struct cs_EnvAction *searchEat(Agent::EnvAction, struct cs_State *,
                const struct cs_Action *) const;
        struct cs_BackwardLink *searchBlk(struct cs_State *,
                const struct cs_State *) const;

        void _deleteState(struct cs_State *); /**< delete state from memory network */
        void deleteAct(Agent::Action, struct cs_State *);
        void deleteEat(Agent::EnvAction, const struct cs_State *,
                struct cs_Action *);
        void deleteBlk(struct cs_State *, struct cs_State *);

        void freeState(struct cs_State *); /**< free a state  */
        void freeAct(struct cs_Action *);
        void freeEat(struct cs_EnvAction *);
        void freeBlk(struct cs_BackwardLink *);
        void freeMemory(); /**< free all space of memory in computer memory*/

        void buildStateFromHeader(const struct State_Info_Header *,
                struct cs_State *);

        float calStatePayoff(const struct cs_State *) const; /**< calculate payoff of a state */
        float calActPayoff(Agent::Action, const struct cs_State *) const; /**< calculate payoff of an Agent::Action */
        float _calActPayoff(const struct cs_Action *) const;
};

/** state information */
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

/** implementation of action information */
struct cs_Action
{
        Agent::Action act; /**< action value */
        struct cs_EnvAction *ealist;

        struct cs_Action *next;
};

/** implementation of forward link information */
struct cs_EnvAction
{
        Agent::EnvAction eat;
        unsigned long count; /**< eact count */
        struct cs_State *nstate; /**< next state */

        struct cs_EnvAction *next;
};

/** implementation of backward link information */
struct cs_BackwardLink
{
        struct cs_State *pstate; /**< previous state */
        struct cs_BackwardLink *next;
};

}    // namespace gamcs
#endif // CSOSAGENT_H_
