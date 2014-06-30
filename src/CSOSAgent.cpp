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

#include <math.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include "gamcs/CSOSAgent.h"
#include "gamcs/Storage.h"
#include "gamcs/StateInfoParser.h"
#include "gamcs/debug.h"
#include "gamcs/platforms.h"

namespace gamcs
{

/**
 * @brief The default constructor.
 *
 * @param [in] i the agent id
 * @param [in] dr the discount rate
 * @param [in] ac the accuracy
 */
CSOSAgent::CSOSAgent(int i, float dr, float ac) :
		OSAgent(i, dr, ac), state_num(0), lk_num(0), head(NULL), cur_mst(NULL), current_st_index(
		NULL)
{
	states_map.clear();
	update_queue.clear();
	visited_states.clear();
}

/**
 * @brief The default destructor.
 */
CSOSAgent::~CSOSAgent()
{
	freeMemory();    // free computer memory
}

/**
 * @brief Load a specified state to computer memory from a previously dumped memory.
 *
 * @param [in] storage the storage where to load the memory
 * @param [in] st the state to be loaded
 */
void CSOSAgent::loadState(Storage *storage, Agent::State st)
{
	State_Info_Header *sthd = storage->getStateInfo(st);
	if (sthd == NULL)    // should not happen, otherwise database corrupted!
		ERROR(
				"state: %" ST_FMT " should exist, but fetch from storage returns NULL, the database may be corrupted!\n",
				st);

	struct cs_State *mst = searchState(st);    // search memory for the state first
	if (mst == NULL)
		addStateInfo(sthd);
	else
		updateStateInfo(sthd);

	free(sthd);
	return;
}

/**
 * @brief Load and initialize memory from a storage.
 *
 * @param [in] storage the storage where to load the memory
 */
void CSOSAgent::loadMemoryFromStorage(Storage *storage)
{
	if (storage == NULL)    // no database specified, do nothing
		return;

	int re = storage->open(Storage::O_READ);    // otherwise, load memory from database
	if (re == 0)    // successfully opened
	{
		char label[10] = "Loading: ";
		printf("Loading Memory from Storage... \n");
		fflush(stdout);

		/* load memory information */
		unsigned long saved_state_num = 0, saved_lk_num = 0;
		struct Memory_Info *memif = storage->getMemoryInfo();
		if (memif != NULL)
		{
			discount_rate = memif->discount_rate;
			accuracy = memif->accuracy;
			saved_state_num = memif->state_num;    // don't use state_num directly
			saved_lk_num = memif->lk_num;    // don't use lk_num directly
			pre_in = memif->last_st;    // it's continuous
			pre_out = memif->last_act;    //
			free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
		}

		/* load states information */
		Agent::State st = storage->firstState();
		unsigned long index = 0;
		while (st != INVALID_STATE)
		{
			dbgmoreprt("LoadMemory()", "LoadState: %ld\n", st);
			loadState(storage, st);

			st = storage->nextState();
			index++;
			pi_progressBar(index, saved_state_num, label);
		}

		// do some check of numbers
		// state number
		if (saved_state_num != state_num)
		{
			WARNNING(
					"LoadMemory(): Number of states not consistent,which by stored meminfo says to be %ld, but in stateinfo is %ld, the storage may be conrupted!\n",
					saved_state_num, state_num);
		}
		// link number
		if (saved_lk_num != lk_num)
		{
			WARNNING(
					"LoadMemory(): Number of links not consistent,which by stored meminfo says to be %ld, but in stateinfo is %ld, the storage may be conrupted!\n",
					saved_lk_num, lk_num);
		}
	}

	storage->close();
	return;
}

/**
 * @brief Dump agent memory to a storage, including states information and memory-level statistics.
 *
 * @param [in] storage the storage where the memory is dumped to
 */
void CSOSAgent::dumpMemoryToStorage(Storage *storage) const
{
	if (storage == NULL)    // no database specified, no need to save
		return;

	char label[10] = "Saving: ";
	printf("Saving Memory to Storage... \n");
	int re = storage->open(Storage::O_WRITE);    // open for writing
	if (re == 0)    // successfully connected
	{
		/* save memory information */
		struct Memory_Info *memif = (struct Memory_Info *) malloc(
				sizeof(struct Memory_Info));
		memif->discount_rate = discount_rate;
		memif->accuracy = accuracy;
		memif->lk_num = lk_num;
		memif->state_num = state_num;
		memif->last_st = pre_in;
		memif->last_act = pre_out;

		storage->addMemoryInfo(memif);    // Add to storage
		free(memif);    // free it

		/* save states information */
		struct State_Info_Header *stif = NULL;
		struct cs_State *mst, *nmst;
		unsigned long index = 0;
		// walk through all state structs
		for (mst = head; mst != NULL; mst = nmst)
		{
			if (storage->hasState(mst->st))
			{
				dbgmoreprt("SaveMemory()", "Update state: %" ST_FMT ", Payoff: %.3f\n", mst->st, mst->payoff);
				stif = getStateInfo(mst->st);
				assert(stif != NULL);
				storage->updateStateInfo(stif);
				free(stif);    // free
			}
			else    // new state
			{
				dbgmoreprt("SaveMemory()", "Add state: %" ST_FMT ", Payoff: %.3f\n", mst->st, mst->payoff);
				stif = getStateInfo(mst->st);
				assert(stif != NULL);
				storage->addStateInfo(stif);
				free(stif);    // free
			}

			index++;
			pi_progressBar(index, state_num, label);
			nmst = mst->next;
		}
	}

	storage->close();
	return;
}

/**
 * @brief Search for a state in memory.
 *
 * @param [in] st the state to be searched
 * @return address pointer of the state if found, or NULL for not existing
 */
struct cs_State *CSOSAgent::searchState(Agent::State st) const
{
	StatesMap::const_iterator it = states_map.find(st);    // find the state value in hash map
	if (it != states_map.end())    // found
		return (struct cs_State *) (it->second);
	else
		return NULL;
}

/**
 * @brief Create a structure in computer memory to represent a state.
 *
 * @param [in] st the state to be created
 * @return address pointer of the new state
 * @see deleteState()
 * @see freeState()
 */
struct cs_State *CSOSAgent::newState(Agent::State st)
{
	struct cs_State *mst = (struct cs_State *) malloc(sizeof(struct cs_State));
	assert(mst != NULL);
	// fill in default values
	mst->st = st;
	mst->original_payoff = 0.0;    // use 0 as default
	mst->payoff = 0.0;
	mst->count = 1;    // it's created when we first encounter it
	mst->actlist = NULL;
	mst->blist = NULL;

	// Add mst to the front of head
	mst->prev = NULL;
	mst->next = head;
	if (head != NULL)
		head->prev = mst;
	head = mst;

	states_map.insert(StatesMap::value_type(mst->st, mst));    // don't forget to update hash map

	state_num++;
	return mst;
}

/**
 * @brief Free a state structure and retrieve its computer memory.
 *
 * @param [in] mst the state to be freed
 * @see deleteState()
 */
void CSOSAgent::freeState(struct cs_State *mst)
{
	if (mst == NULL)
	{
		return;
	}
	// before free the struct itself, free its subparts first
	/* free actlist */
	struct cs_Action *ac, *nac;
	for (ac = mst->actlist; ac != NULL; ac = nac)
	{
		nac = ac->next;
		freeAct(ac);
	}

	/* free blist */
	struct cs_BackwardLink *bas, *nbas;
	for (bas = mst->blist; bas != NULL; bas = nbas)
	{
		nbas = bas->next;
		freeBlk(bas);
	}

	return free(mst);
}

/**
 * @brief Create a structure in computer memory to represent an environment action.
 *
 * @param [in] eat the environment action to be created
 * @param [in] nst the following state of the environment action
 * @param [in] mac the action which the environment action is belonged to
 * @return address pointer of the new environment action
 * @see deleteEat()
 * @see freeEat()
 */
struct cs_EnvAction *CSOSAgent::newEat(EnvAction eat, struct cs_State *nst,
		struct cs_Action *mac)
{
	struct cs_EnvAction *meat = (struct cs_EnvAction *) malloc(
			sizeof(struct cs_EnvAction));
	meat->eat = eat;
	meat->count = 1;
	meat->nstate = nst;

	// add to ealist
	meat->next = mac->ealist;
	mac->ealist = meat;

	lk_num++;    // increase link number
	return meat;
}

/**
 * @brief Free an environment action structure and retrieve its computer memory.
 *
 * @param [in] meat the environment action to be freed
 * @see deleteEat()
 */
void CSOSAgent::freeEat(struct cs_EnvAction *meat)
{
	return free(meat);
}

/**
 * @brief Create a new backward link.
 *
 * @param [in] pmst the following state
 * @param [in] mst the state which has pmst as its following state
 * @return address pointer of the new backward link
 * @see deleteBlk()
 * @see freeBlk()
 */
struct cs_BackwardLink *CSOSAgent::newBlk(struct cs_State *pmst,
		struct cs_State *mst)
{
	// state shouldn't repeat in backward list, check if already exists
	struct cs_BackwardLink *bas = searchBlk(pmst, mst);

	if (bas == NULL)    // not found, create a new one and Add to blist
	{
		bas = (struct cs_BackwardLink *) malloc(sizeof(struct cs_BackwardLink));
		bas->pstate = pmst;    // previous state is mst
		// Add to blist
		bas->next = mst->blist;
		mst->blist = bas;
	}

	return bas;
}

/**
 * @brief Free a backward link.
 * @see deleteBlk()
 */
void CSOSAgent::freeBlk(struct cs_BackwardLink *bas)
{
	return free(bas);
}

/**
 * @brief Search for action under a specified state.
 *
 * @param [in] act the action to be searched
 * @param [in] mst the state under which the action is searched
 * @return address pointer of the action if found, or NULL if not
 */
struct cs_Action* CSOSAgent::searchAct(Agent::Action act,
		const struct cs_State *mst) const
{
	struct cs_Action *ac, *nac;
	// walk through action list
	for (ac = mst->actlist; ac != NULL; ac = nac)
	{
		if (ac->act == act)
			return ac;

		nac = ac->next;
	}

	return NULL;
}

/**
 * @brief Search for environment action under a specified action.
 *
 * @param [in] eat the environment action to be searched
 * @param [in] nmst the following state which the environment action should contain
 * @param [in] mac the actions under which the environment action is searched
 * @return address pointer of the environment action if found, or NULL if not
 */
struct cs_EnvAction *CSOSAgent::searchEat(EnvAction eat, struct cs_State *nmst,
		const cs_Action *mac) const
{
	struct cs_EnvAction *meat, *nmeat;
	for (meat = mac->ealist; meat != NULL; meat = nmeat)
	{
		if (meat->eat == eat && meat->nstate == nmst)
			return meat;

		nmeat = meat->next;
	}

	return NULL;
}

/**
 * @brief Search for backward link.
 *
 * @param [in] pmst the following state
 * @param [in] mst the up-streaming state
 * @return address pointer of the backward link if found, or NULL if not
 */
struct cs_BackwardLink *CSOSAgent::searchBlk(struct cs_State *pmst,
		const struct cs_State *mst) const
{
	struct cs_BackwardLink *bas, *nbas;
	for (bas = mst->blist; bas != NULL; bas = nbas)
	{
		if (bas->pstate == pmst)    // found
			return bas;

		nbas = bas->next;
	}

	return NULL;
}

/**
 * @brief Delete an action from a state.
 *
 * @param [in] act the action to be deleted
 * @param [in] mst the state which the action is belonged to
 * @see newAct()
 * @see freeAct()
 */
void CSOSAgent::deleteAct(Agent::Action act, struct cs_State *mst)
{
	struct cs_Action *tmp, *prev = NULL;
	struct cs_EnvAction *meat, *nmeat;
	tmp = mst->actlist;
	while (tmp != NULL)
	{
		if (tmp->act == act)    // found
		{
			if (tmp == mst->actlist)    // it's head
			{
				mst->actlist = tmp->next;
				// update link number
				for (meat = tmp->ealist; meat != NULL; meat = nmeat)
				{
					lk_num--;    // every link from this act will be deleted
					nmeat = meat->next;
				}
				freeAct(tmp);    // then free itself
				return;
			}
			else
			{
				prev->next = tmp->next;
				// update link number
				for (meat = tmp->ealist; meat != NULL; meat = nmeat)
				{
					lk_num--;    // every link from this act will be deleted
					nmeat = meat->next;
				}
				freeAct(tmp);
				return;
			}
		}
		else    // check next act
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}
	return;
}

/**
 * @brief Delete an environment action from an action.
 *
 * @param [in] eat the environment action to be deleted
 * @param [in] nst the following state which the environment action should contain
 * @param [in] mac the action which the environment action is belonged to
 * @see newEat()
 * @see freeEat()
 */
void CSOSAgent::deleteEat(Agent::EnvAction eat, const struct cs_State *nst,
		struct cs_Action *mac)
{
	struct cs_EnvAction *tmp, *prev = NULL;
	tmp = mac->ealist;
	while (tmp != NULL)
	{
		if (tmp->eat == eat && tmp->nstate == nst)    // found
		{
			if (tmp == mac->ealist)    // head
			{
				mac->ealist = tmp->next;
				freeEat(tmp);
				lk_num--;    // decrease link number
				return;
			}
			else
			{
				prev->next = tmp->next;
				freeEat(tmp);
				lk_num--;    // decrease link number
				return;
			}
		}
		else
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}

	return;
}

/**
 * @brief Delete a backward link between two specified states.
 *
 * @param [in] pmst the up-streaming state
 * @param [in] mst the following state
 * @see newBlk()
 * @see freeBlk()
 */
void CSOSAgent::deleteBlk(struct cs_State *pmst, struct cs_State *mst)
{
	struct cs_BackwardLink *tmp, *prev = NULL;
	tmp = mst->blist;
	while (tmp != NULL)
	{
		if (tmp->pstate == pmst)    // found
		{
			if (tmp == mst->blist)    // head
			{
				mst->blist = tmp->next;
				freeBlk(tmp);
				return;
			}
			else
			{
				prev->next = tmp->next;
				freeBlk(tmp);
				return;
			}
		}
		else
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}
	return;
}

/**
 * @brief Create a structure in computer memory to represent an action.
 *
 * @param [in] act the action to be created
 * @param [in] mst the state which the action is belonged to
 * @return address pointer of the new action
 * @see deleteAct()
 * @see freeAct()
 */
struct cs_Action *CSOSAgent::newAct(Agent::Action act, struct cs_State *mst)
{
	struct cs_Action *mac = (struct cs_Action *) malloc(
			sizeof(struct cs_Action));

	mac->act = act;
	mac->ealist = NULL;

	// add to actlist
	mac->next = mst->actlist;
	mst->actlist = mac;
	return mac;
}

/**
 * @brief Free an action structure and retrieve its computer memory.
 *
 * @param [in] ac the action to be freed
 * @see newAct()
 * @see deleteAct()
 */
void CSOSAgent::freeAct(struct cs_Action *ac)
{
	// free ealist
	struct cs_EnvAction *meat, *nmeat;
	for (meat = ac->ealist; meat != NULL; meat = nmeat)
	{
		nmeat = meat->next;
		freeEat(meat);
	}

	return free(ac);
}

/**
 * @brief Create a link between a state and its following state.
 *
 * mst + eat + act ==> nmst
 * @param [in] mst the state where the link starts
 * @param [in] eat the environment action of this link
 * @param [in] act the action of this link
 * @param [in] nmst the following state of this link
 */
void CSOSAgent::linkStates(struct cs_State *mst, EnvAction eat,
		Agent::Action act, struct cs_State *nmst)
{
	dbgmoreprt("Enter LinkStates()", "------------------- Make Link: %" ST_FMT " == %" ACT_FMT " + %" ACT_FMT " => %" ST_FMT "\n", mst->st, eat, act, nmst->st);

	// mst->st + eat + act == nmst->st!
//    assert(mst->st + eat + act == nmst->st);

	struct cs_Action *mac;
	struct cs_EnvAction *meat;

	/* check if the link already exists, if so simply update the count of environment action */
	mac = searchAct(act, mst);
	if (mac != NULL)
	{
		meat = searchEat(eat, nmst, mac);
		if (meat != NULL)    // link exists
		{
			dbgmoreprt("LinkStates():", "link already exists, increase count only\n");
			meat->count++;    // increase count
			return;    // done, return
		}
		else    // act exists, but eat not exist, meat == NULL
		{
			dbgmoreprt("LinkStates():", "create new link...\n");
			meat = newEat(eat, nmst, mac);
		}
	}
	else    // act not exist
	{
		dbgmoreprt("LinkStates():", "create new link...\n");
		mac = newAct(act, mst);

		meat = newEat(eat, nmst, mac);
	}

	newBlk(mst, nmst);    // build the backward link
	return;
}

/**
 * @brief Calculate the possibility of encountering an specified environment action.
 *
 * @param [in] ea the environment action
 * @param [in] mac the action which the environment action is belonged to
 * @return the calculated possibility [0, 1]
 */
float CSOSAgent::prob(const struct cs_EnvAction *ea,
		const struct cs_Action *mac) const
{
	unsigned long eacount = ea->count;
	// calculate the sum of env action counts
	unsigned long sum_eacount = 0;
	struct cs_EnvAction *pea, *pnea;
	for (pea = mac->ealist; pea != NULL; pea = pnea)
	{
		sum_eacount += pea->count;

		pnea = pea->next;
	}

	// state count donesn't equal to sum of eacount due to the set operation (actually state count will become smaller than sum eacount gradually)
	dbgmoreprt("Prob", "------- action: %" ACT_FMT "\n", mac->act);dbgmoreprt("Prob", "sum: %ld\n", sum_eacount);

	float re = (1.0 / sum_eacount) * eacount;    // number of env actions divided by the total number
	/* do some checks below */
	// check if re is in range (0, 1]
	if (re < 0 || re > 1)    // check failed
	{
		ERROR(
				"Prob(): probability is %.2f, which must in range [0, 1]. action: %" ACT_FMT ", eact is %" ACT_FMT ", count is %ld, total eacount is %ld.\n",
				re, mac->act, ea->eat, eacount, sum_eacount);
	}

	return re;
}

/**
 * @brief Calculate the payoff of the specified state.
 *
 * $ u(I_i) = u_0(I_i) + \eta * MAX_{O^j_i\in \Lambda_i}(\sum_{k=1}^m{P(E^k_i|O^j_i)*u(I^{k,j}_i)) $
 * @param [in] mst the state to be calculated
 * @return payoff of the state
 * @see calActPayoff()
 */
float CSOSAgent::calStatePayoff(const struct cs_State *mst) const
{
	dbgmoreprt("\nCalStatePayoff()", "----------------- state: %" ST_FMT ", count: %ld\n", mst->st, mst->count);

	float u0 = mst->original_payoff;

	if (mst->actlist == NULL)    // no any actions, return u0
		return trimPayoff(u0);   // trim the payoff

	// find the maximun action payoff
	float payoff = 0;
	register float max_pf = -FLT_MAX, action_payoff = 0;
	struct cs_Action *mac, *nmac;
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		action_payoff = _calActPayoff(mac);
		if (action_payoff > max_pf)
		{
			max_pf = action_payoff;
		}

		nmac = mac->next;
	}

	payoff = u0 + discount_rate * max_pf;

	return trimPayoff(payoff);  // trim the payoff
}

/**
 * @brief Update states starting from a specified state backwardly.
 *
 * Note that: every time a state makes any changes, all its up-streaming states must be updated!
 * @param [in] mst the state where the update starts
 */
void CSOSAgent::updateStatePayoff(cs_State *mst)
{
	// clear update_queue and visited_states first every time
	update_queue.clear();
	visited_states.clear();

	update_queue.push_back(mst);    // add the starting state

	cs_State *cmst = NULL;
	register float payoff = 0.0;
	struct cs_BackwardLink *bas, *nbas;

	while (!update_queue.empty())
	{
		cmst = update_queue.front();    // get the state at front
		payoff = calStatePayoff(cmst);

		if (cmst->payoff != payoff)    // the backtrace will stop at where the payoff won't change
		{
			cmst->payoff = payoff;
			dbgmoreprt("UpdateState()", "State: %" ST_FMT " change to payoff: %.3f\n", cmst->st, payoff);

			// push previous states to queue
			for (bas = cmst->blist; bas != NULL; bas = nbas)
			{
				// visited state will not be pushed
				if (visited_states.find(bas->pstate) == visited_states.end())    // not found
				{
					update_queue.push_back(bas->pstate);
				}
				nbas = bas->next;
			}
		}
		else
		{
			dbgmoreprt("UpdateState()", "State: %" ST_FMT ", payoff no changes, update stopped here.\n", cmst->st);
		}

		visited_states.insert(cmst);    // save visited state
		update_queue.pop_front();    // remove the state at front
	}
}

/**
 * @brief Calculate the payoff of a specified action.
 *
 * @param [in] act the action to be calculated
 * @param [in] mst the state which the action is belonged to
 * @return payoff of the action
 * @see calStatePayoff()
 */
float CSOSAgent::calActPayoff(Agent::Action act,
		const struct cs_State *mst) const
{
	cs_Action *mac = searchAct(act, mst);
	if (mac == NULL)    // this is an unseen action
		return 0.0;    // 0 for unseen action

	return _calActPayoff(mac);
}

/**
 * @brief Calculate the payoff of a specified action.
 *
 * $ u(O^j_i) = \sum_{k=1}^m{P(E^k_i|O^j_i) * u(I^{k,j}_i)} $
 * @param [in] mac the address pointer of the action to be calculated
 * @return payoff of the action
 * @see calStatePayoff()
 **/
float CSOSAgent::_calActPayoff(const cs_Action *mac) const
{
	register float payoff = 0;

	struct cs_EnvAction *ea, *nea;
	for (ea = mac->ealist; ea != NULL; ea = nea)
	{
		payoff += prob(ea, mac) * ea->nstate->payoff;

		nea = ea->next;
	}

	return trimPayoff(payoff);  // trim the payoff
}

/**
 * @brief Trim payoff value according to the accuracy.
 *
 * @param [in] pf the payoff
 * @return the trimed payoff
 */
float CSOSAgent::trimPayoff(float pf) const
{
    if (accuracy == 0.0)
    {
        return pf;  // no trim when accuracy is 0, the accuracy of payoff is dertermined by the bits of float
    }
    else
    {
        return  floor(pf / accuracy) * accuracy;
    }
}

/**
 * @brief Find and choose the best actions of a state from the action space.
 *
 * @param [in] mst the state
 * @param [in] acts the action space of the state
 * @return the best actions
 */
OSpace CSOSAgent::bestActions(const struct cs_State *mst, OSpace &acts) const
{
	register float max_payoff = -FLT_MAX, payoff = 0;
	OSpace best_acts;

	best_acts.clear();
	// walk through every action in list
	register Agent::Action act = acts.first();
	while (act != INVALID_ACTION)    // until out of bound
	{
		payoff = calActPayoff(act, mst);

		if (payoff > max_payoff)    // find a bigger one, refill the max payoff action list
		{
			best_acts.clear();
			best_acts.add(act);
			max_payoff = payoff;
		}
		else if (payoff == max_payoff)    // find an equal one, Add it to the list
			best_acts.add(act);

		act = acts.next();
	}
	return best_acts;
}

/**
 * @brief Update states in memory.
 *
 * Note: This function should be called AFTER MaxPayoffRule() in every step!
 * @param [in] oripayoff original payoff of current state
 */
void CSOSAgent::updateMemory(float oripayoff)
{
	dbgmoreprt("\nEnter UpdateMemory()", "-------------------------------------------------\n");
	// In EXPLORE/PASSIVE mode, maxPayoffRule() will not run, which leaves cur_mst unset, so we have to set cur_mst here
	// otherwise, cur_mst will be set by maxPayoffRule().
	// FIXME: this reduces time to search but is a bit ugly!
	if (learning_mode == EXPLORE || learning_mode == PASSIVE)
		cur_mst = searchState(cur_in);

	if (pre_in == INVALID_STATE)    // previous state not exist, it's running for the first time
	{
		dbgmoreprt("", "Previous state not exists, create current state in memory.\n");
		//NOTE: cur_mst is already set in MaxPayoffRule() function
		if (cur_mst == NULL)    // create current state in memory
		{
			cur_mst = newState(cur_in);
			if (oripayoff != INVALID_PAYOFF)
				cur_mst->original_payoff = oripayoff;    // set original payoff as given if it's valid, else no changes
		}
		else    // state found, this could happen if others send state information to me before the first time I'm running
		{
			dbgmoreprt("", "Previous state not exists, but I recieved some information of this state from others.\n");
			// update current state
			cur_mst->count++;    // inc state count
			if (oripayoff != INVALID_PAYOFF)
				cur_mst->original_payoff = oripayoff;    // reset original payoff
			// no previous state, so no link involved
		}

		return;
	}

	dbgmoreprt("", "Previous state is %ld.\n", pre_in);
	/* previous state exists */
	struct cs_State *pmst = searchState(pre_in);    // found previous state struct
	if (pmst == NULL)
		ERROR(
				"UpdateMemory(): Can not find previous state %" ST_FMT " in memory, which should be existing!\n",
				pre_in);

	//NOTE: cur_mst is already set in MaxPayoffRule() function
	if (cur_mst == NULL)    // currrent state struct not exists in memory, create it in memory, and link it to the previous state
	{
		dbgmoreprt("", "current state not exists, create it and build the link\n");
		cur_mst = newState(cur_in);
		if (oripayoff != INVALID_PAYOFF)
			cur_mst->original_payoff = oripayoff;

		// build the link
		EnvAction peat = cur_in - pre_in - pre_out;    // calcuate previous environment action. This formula is important!!!
		linkStates(pmst, peat, pre_out, cur_mst);    // build the link
	}
	else    // current state struct already exists, update the count and link it to the previous state (LinkStates will handle it if the link already exists.)
	{
		dbgmoreprt("", "current state is %" ST_FMT ", increase count and build the link\n", cur_mst->st);
		// update current state
		cur_mst->count++;    // inc state count
		if (oripayoff != INVALID_PAYOFF)
			cur_mst->original_payoff = oripayoff;    // reset original payoff

		// build the link
		EnvAction peat = cur_in - pre_in - pre_out;
		linkStates(pmst, peat, pre_out, cur_mst);
	}

	// update payoff starting from previous states recursively
	// DO NOT start from current state, the previous states need to
	// be updated because of the new created link to current state!
	struct cs_BackwardLink *blk, *nblk;
	for (blk = cur_mst->blist; blk != NULL; blk = nblk)
	{
		updateStatePayoff(blk->pstate);

		nblk = blk->next;
	}

	return;
}

/**
 * @brief Free the whole computer memory used by an agent.
 */
void CSOSAgent::freeMemory()
{
	// free all states in turn
	struct cs_State *mst, *nmst;
	for (mst = head; mst != NULL; mst = nmst)
	{
		nmst = mst->next;
		freeState(mst);
	}

	states_map.clear();
	update_queue.clear();
	visited_states.clear();
}

/**
 * @brief Delete and free a specified state from memory.
 *
 * @param [in] mst the state to be deleted
 */
void CSOSAgent::_deleteState(struct cs_State *mst)
{
	if (mst == NULL)
	{
		return;
	}

	// first, remove mst from previous states' forward links
	struct cs_BackwardLink *blk, *nblk;
	struct cs_Action *mac, *nmac;
	struct cs_EnvAction *meat, *nmeat;
	struct cs_State *pmst, *nmst;
	for (blk = mst->blist; blk != NULL; blk = nblk)
	{
		pmst = blk->pstate;
		for (mac = pmst->actlist; mac != NULL; mac = nmac)
		{
			Agent::EnvAction eat = mst->st - pmst->st - mac->act;    // calculate the possible eat
			deleteEat(eat, mst, mac);

			nmac = mac->next;
		}

		nblk = blk->next;
	}

	// then, remove mst from following states' backward links
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		for (meat = mac->ealist; meat != NULL; meat = nmeat)
		{
			nmst = meat->nstate;
			deleteBlk(mst, nmst);

			nmeat = meat->next;
		}

		nmac = mac->next;
	}

	// update lk_num
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		for (meat = mac->ealist; meat != NULL; meat = nmeat)
		{
			lk_num--;    // every link from mst will be deleted
			nmeat = meat->next;
		}
		nmac = mac->next;
	}

	// free the state itself
	// remove state from the double link
	if (head == mst)
		head = mst->next;    // it's head
	if (mst->prev != NULL)
		mst->prev->next = mst->next;
	if (mst->next != NULL)
		mst->next->prev = mst->prev;

	// remove state from hash map
	states_map.erase(mst->st);
	state_num--;

	return freeState(mst);
}

/**
 * @brief Implementation of the Maximum Payoff Rule (MPR).
 *
 * @param [in] st the state which is concerned
 * @param [in] acts the action space of the state
 * @return the actions generated by MPR
 * @see bestActions()
 */
OSpace CSOSAgent::maxPayoffRule(Agent::State st, OSpace &acts) const
{
	dbgmoreprt("Enter MaxPayoffRule() ", "---------------------- State: %" ST_FMT "\n", st);
	cur_mst = searchState(st);    // get the state struct from state value

	if (cur_mst == NULL)    // first time to encounter this state, we know nothing about it, so no restriction applied, return the whole list
	{
		dbgmoreprt("MaxPayoffRule()", "State not found in memory.\n");
		return acts;
	}
	else    // we have memories about this state, find the best action of it
	{
		return bestActions(cur_mst, acts);
	}
}

/**
 * @brief Open the agent for read or write.
 *
 * @param [in] flag the open flag
 * @return 0 for successfully opened, or -1 if error occurs
 * @see close()
 */
int CSOSAgent::open(Flag flag)
{
	if (flag == O_READ)
	{
		return 0;    // nothing to do
	}
	else if (flag == O_WRITE)
	{
		return 0;
	}
	else
	{
		WARNNING("Unknown storage open flag: %d!\n", flag);
		return -1;
	}
}

/**
 * @brief Close the agent.
 * @see open(Flag)
 */
void CSOSAgent::close()
{
	return;    // nothing to do
}

/**
 * @brief Get the information of a specified state.
 *
 * @param [in] st the state whose information is to get
 * @return address pointer of state information header, or NULL if error occurs
 */
struct State_Info_Header *CSOSAgent::getStateInfo(Agent::State st) const
{
	if (st == INVALID_STATE)    // check if valid
	{
		dbgmoreprt("GetStateInfo()", "invalid state value\n");
		return NULL;
	}

	struct cs_State *mst;
	mst = searchState(st);    // find the state struct in computer memory

	if (mst == NULL)    // not found
	{
		dbgmoreprt("GetStateInfo()", "state: %" ST_FMT " not found in memory!\n", st);
		return NULL;
	}

	// get the total size of state
	int sthd_size = sizeof(State_Info_Header);    // add state info header size
	int act_num = 0;
	struct cs_Action *mac, *nmac;
	struct cs_EnvAction *ea, *nea;
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		sthd_size += sizeof(Action_Info_Header);    // add up each action info header size
		for (ea = mac->ealist; ea != NULL; ea = nea)
		{
			sthd_size += sizeof(EnvAction_Info);    // add up each eat info size of each action

			nea = ea->next;
		}

		act_num++;
		nmac = mac->next;
	}

	State_Info_Header *sthd = (State_Info_Header *) malloc(sthd_size);
	assert(sthd != NULL);
	// fill the header
	sthd->st = st;
	sthd->original_payoff = mst->original_payoff;
	sthd->payoff = mst->payoff;
	sthd->count = mst->count;
	sthd->act_num = act_num;
	sthd->size = sthd_size;

	unsigned char *stp = (unsigned char *) sthd;    // use point stp to travel through each subpart of state
	unsigned char *atp = NULL;
	stp += sizeof(State_Info_Header);    // point to the first act
	Action_Info_Header *athd = NULL;
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		athd = (Action_Info_Header *) stp;
		athd->act = mac->act;    // fill action value

		atp = stp + sizeof(Action_Info_Header);    // point to the first eat of act
		int ea_num = 0;
		EnvAction_Info *eaif = NULL;
		for (ea = mac->ealist; ea != NULL; ea = nea)
		{
			eaif = (EnvAction_Info *) atp;
			eaif->eat = ea->eat;    // fill env action info
			eaif->count = ea->count;
			eaif->nst = ea->nstate->st;

			ea_num++;
			atp += sizeof(EnvAction_Info);    // point to the next eat
			nea = ea->next;
		}

		athd->eat_num = ea_num;    // fill eat number

		stp += sizeof(Action_Info_Header)
				+ athd->eat_num * sizeof(EnvAction_Info);    // point to the next act
		nmac = mac->next;
	}

	return sthd;
}

/**
 * @brief Build a state structure in computer memory from a state information.
 *
 * @param [in] sthd the state information header
 * @param [in] mst the state structure to be built in memory
 */
void CSOSAgent::buildStateFromHeader(const State_Info_Header *sthd,
		cs_State *mst)
{
	// copy state information
	mst->count = sthd->count;
	mst->payoff = sthd->payoff;
	mst->original_payoff = sthd->original_payoff;    // the original payoff is what really is important

	// copy actlist
	StateInfoParser sparser(sthd);
	Action_Info_Header *athd = NULL;
	EnvAction_Info *eaif = NULL;

	athd = sparser.firstAct();
	while (athd != NULL)
	{
		// create this act and add it to state
		cs_Action *mac = newAct(athd->act, mst);

		eaif = sparser.firstEat();
		while (eaif != NULL)
		{
			// check links to the next state
			struct cs_State *nmst = searchState(eaif->nst);    // find if the next state exists
			if (nmst != NULL)    // if so, inc count and make the link
			{
				dbgmoreprt("next state", "%" ST_FMT " exists, build the link\n", eaif->nst);
			}
			else    // for a non-existing next state, we will create it, and build the link
			{
				// create a new previous state
				dbgmoreprt("next state", "%" ST_FMT " not exists, create it and build the link\n", eaif->nst);
				nmst = newState(eaif->nst);
			}
			// build the link
			// create this eat and add it to act
			cs_EnvAction *meat = newEat(eaif->eat, nmst, mac);
			meat->count = eaif->count;    // copy eat count
			newBlk(mst, nmst);    // build backward link

			eaif = sparser.nextEat();    // next eat
		}

		athd = sparser.nextAct();    // next act
	}

	return;
}

/**
 * @brief Add a state from a state information to computer memory.
 *
 * Make sure the state is not existing in memory before adding, otherwise use updateStateInfo() instead.
 * @param [in] sthd the state information to be added
 */
void CSOSAgent::addStateInfo(const State_Info_Header *sthd)
{
#ifdef _DEBUG_MORE_
	printf("---------------------- AddStateInfo: ---------------------\n");
	PrintStateInfo(sthd);
#endif

	struct cs_State *mst = searchState(sthd->st);    // search for the state
	if (mst != NULL)    // state already exists, use UpdateStateInfo() instead!
	{
		WARNNING(
				"AddStateInfo(): state %" ST_FMT " already exists in memory, if you want to change it, using UpdateStateInfo()!\n",
				sthd->st);
		return;
	}

	dbgmoreprt("AddStateInfo()",
			"state: %" ST_FMT ", create it in memory.\n", sthd->st);

	mst = newState(sthd->st);

	buildStateFromHeader(sthd, mst);

	return;
}

/**
 * @brief Update a state from a state information in computer memory.
 *
 * Make sure the state is already existing in memory before updating, otherwise use addStateInfo() instead.
 * @param [in] sthd the state information to be updated
 */
void CSOSAgent::updateStateInfo(const State_Info_Header *sthd)
{
#ifdef _DEBUG_MORE_
	printf("---------------------- UpdateStateInfo: ---------------------\n");
	PrintStateInfo(sthd);
#endif

	struct cs_State *mst = searchState(sthd->st);    // search for the state
	if (mst == NULL)    // state doesn't exists, use AddStateInfo() instead!
	{
		WARNNING(
				"UpdateStateInfo(): state %" ST_FMT " doesn't exist in memory, if you want to add it, using AddStateInfo()!\n",
				sthd->st);
		return;
	}

	dbgmoreprt("UpdateStateInfo()", "update information of state %" ST_FMT ".\n", sthd->st);

	// clear state first
	struct cs_Action *mac, *nmac;
	struct cs_EnvAction *meat, *nmeat;
	for (mac = mst->actlist; mac != NULL; mac = nmac)
	{
		for (meat = mac->ealist; meat != NULL; meat = nmeat)
		{
			lk_num--;    // decrease link number for each eat
			nmeat = meat->next;
		}

		nmac = mac->next;
		freeAct(mac);
	}
	mst->actlist = NULL;    // set as NULL! It's very important!

	buildStateFromHeader(sthd, mst);

	return;
}

/**
 * @brief Delete a state from memory.
 *
 * @param [in] st the state to be deleted
 */
void CSOSAgent::deleteState(State st)
{
	struct cs_State *mst = searchState(st);
	return _deleteState(mst);
}

/**
 * @brief Update payoffs starting from a specified state.
 *
 * @param [in] st the state where the update starts
 */
void CSOSAgent::updatePayoff(State st)
{
	struct cs_State *mst = searchState(st);
	return updateStatePayoff(mst);
}

/**
 * @brief Add Memory information to memory.
 *
 * @param [in] memif the memory information to be added.
 */
void CSOSAgent::addMemoryInfo(const struct Memory_Info *memif)
{
	UNUSED(memif);    // memory info is generated by processing, can't be directly changed.
	return;
}

/**
 * @brief Update memory information in memory.
 *
 * @param [in] memif the memory information to be updated
 */
void CSOSAgent::updateMemoryInfo(const struct Memory_Info *memif)
{
	UNUSED(memif);
	return;
}

/**
 * @brief Get the memory information from memory.
 *
 * @return address pointer of the memory information
 */
struct Memory_Info *CSOSAgent::getMemoryInfo() const
{
	struct Memory_Info *memif = (struct Memory_Info *) malloc(
			sizeof(struct Memory_Info));

	memif->discount_rate = discount_rate;
	memif->accuracy = accuracy;
	memif->state_num = state_num;
	memif->lk_num = lk_num;
	memif->last_st = pre_in;
	memif->last_act = pre_out;

	return memif;
}

/**
 * @brief Get the name of memory.
 *
 * @return the memory name
 */
std::string CSOSAgent::getMemoryName() const
{
	char name[64];
	sprintf(name, "Memory_of_Agent_%d", id);
	return name;
}

/**
 * @brief Get the first state in memory.
 *
 * @return the first state value
 * @see nextState()
 */
Agent::State CSOSAgent::firstState() const
{
	current_st_index = head;
	if (current_st_index != NULL)
		return current_st_index->st;
	else
		return INVALID_STATE;
}

/**
 * @brief Get the next state in memory.
 *
 * @return the state value
 * @see firstState()
 */
Agent::State CSOSAgent::nextState() const
{
	if (current_st_index != NULL)
	{
		current_st_index = current_st_index->next;
		if (current_st_index != NULL)
			return current_st_index->st;
		else
			return INVALID_STATE;
	}
	else
		return INVALID_STATE;
}

/**
 * @brief Check if a state exists in memory.
 *
 * @return true|false
 */
bool CSOSAgent::hasState(State st) const
{
	struct cs_State *mst = searchState(st);
	if (mst == NULL)
		return false;
	else
		return true;
}

}    // namespace gamcs
