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

CSOSAgent::CSOSAgent(int i, float dr, float th) :
		OSAgent(i, dr, th), state_num(0), lk_num(0), head(NULL), cur_mst(NULL), current_st_index(
		NULL)
{
	states_map.clear();
	update_queue.clear();
	visited_states.clear();
}

CSOSAgent::~CSOSAgent()
{
	freeMemory();    // free computer memory
}

/** \brief Load a specified state from a previous memory stored in database.
 *
 * \param st state value
 * \return state struct of st in computer memory
 *
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

/** \brief Load and initialize memory from a storage.
 *
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
			threshold = memif->threshold;
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

/** \brief Dump memory to a storage, including states information and memory-level statistics.
 *
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
		memif->threshold = threshold;
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

/** \brief Search for a state in memory
 *
 * \param st state value
 * \return state structure point, NULL for not exists
 *
 */
struct cs_State *CSOSAgent::searchState(Agent::State st) const
{
	StatesMap::const_iterator it = states_map.find(st);    // find the state value in hash map
	if (it != states_map.end())    // found
		return (struct cs_State *) (it->second);
	else
		return NULL;
}

/** \brief Create a new state structure
 * \param st state value to be created
 * \return newly created state structure
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

/** \brief Free a state struct and retrieve its computer memory
 * \param mst state struct
 *
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

/** \brief Create a new forward arc struct
 *
 * \param eat the environment action
 * \param act the action
 * \return a new forward arc struct
 *
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

/** \brief Free a forward arc struct
 *
 * \param fas the struct to be freed
 *
 */
void CSOSAgent::freeEat(struct cs_EnvAction *meat)
{
	return free(meat);
}

/** \brief Create a new Back Arc struct
 *
 * \return a new back arc struct
 *
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

/** \brief Free a back arc struct
 *
 */
void CSOSAgent::freeBlk(struct cs_BackwardLink *bas)
{
	return free(bas);
}

/** \brief Convert from an action value to a action struct
 *
 * \param act action value
 * \param mst in which state struct to search
 * \return the action struct found, NULL if not found
 *
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

/** \brief Create a new action struct
 *
 * \param eat action value
 * \return a new action struct
 *
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

/** \brief Build a link between two states
 * pmst + eat + act ==> mst
 * \param pmst previous state struct
 * \param eat environment action struct of the link
 * \param act action struct of the link
 * \param mst state struct
 *
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
 * \brief Calculate the possibility of encountering an specified environment action
 * \param ea environment
 * \param mst the state struct
 * \return the possibility
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
 * \brief Calculate payoff of the specified state
 *
 * $$u(I_i) = u_0(I_i) + \eta * MAX_{O^j_i\in \Lambda_i}(\sum_{k=1}^m{P(E^k_i|O^j_i)*u(I^{k,j}_i))$$
 *
 * \param mst specified state
 * \return payoff of the state
 */
float CSOSAgent::calStatePayoff(const struct cs_State *mst) const
{
	dbgmoreprt("\nCalStatePayoff()", "----------------- state: %" ST_FMT ", count: %ld\n", mst->st, mst->count);

	float u0 = mst->original_payoff;

	if (mst->actlist == NULL)    // no any actions, return u0
		return u0;

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
	return payoff;
}

/**
 * \brief Update states backwards beginning from a specified state
 * Note that: every time a state makes any changes, all its previous states must be updated!
 * \param mst a specified state where the update begins
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

		if (fabsf(cmst->payoff - payoff) >= threshold)    // states are updated only when diff exceeds threshold
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
			dbgmoreprt("UpdateState()", "State: %" ST_FMT ", payoff no changes, it's smaller than threshold\n", cmst->st);
		}

		visited_states.insert(cmst);    // save visited state
		update_queue.pop_front();    // remove the state at front
	}
}

/**
 * \brief Calculate payoff of a specified action of a state
 * \param act action value
 * \param mst state struct which contains the action
 * \return payoff of the action
 */
float CSOSAgent::calActPayoff(Agent::Action act,
		const struct cs_State *mst) const
{
	cs_Action *mac = searchAct(act, mst);
	if (mac == NULL)    // this is an unseen action
		return 0;    // 0 for unseen action

	return _calActPayoff(mac);
}

/**
 * $$u(O^j_i) = \sum_{k=1}^m{P(E^k_i|O^j_i) * u(I^{k,j}_i)}$$
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
	return payoff;
}

/**
 * \brief Choose the best actions of a state from a candidate action list.
 * \param mst the state
 * \param candidate action list
 * \return best actions
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
 * \brief Update states in memory. Note: This function should be called AFTER MaxPayoffRule() in every step!
 * \param oripayoff original payoff of current state
 */
void CSOSAgent::updateMemory(float oripayoff)
{
	dbgmoreprt("\nEnter UpdateMemory()", "-------------------------------------------------\n");
	// In EXPLORE mode, maxPayoffRule() will not run, which leaves cur_mst unset, so we have to set cur_mst here
	// otherwise, cur_mst will be set by maxPayoffRule().
	// FIXME: this reduces time to search but is a bit ugly!
	if (learning_mode == EXPLORE)
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
 * \brief Free computer memory used by agent.
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
 * \brief Remove and free a specified state from agent's memory.
 *
 * \param mst the state to be removed
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
 * \brief Implementation of the Maximum Payoff Rule (MPR).
 * \param st state which is concerned
 * \param acts the candidate action list
 * \return actions generated by MPR
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

void CSOSAgent::close()
{
	return;    // nothing to do
}

/**
 * \brief Get information of specified state from memory
 * \param st the state whose information is requested
 * \return the header pointed to the state information, NULL for error
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

/** \brief Add a state to memory.
 * Make sure the state is not existing in memory before adding.
 * \param sthd information header of the new state to be added
 *
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

/** \brief Update a state in memory.
 * Make sure the state is already existing in memory before updating.
 * \param sthd information header of the state to be updated
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

/** \brief Delete a state from memory.
 *  \param st the state to be deleted
 */
void CSOSAgent::deleteState(State st)
{
	struct cs_State *mst = searchState(st);
	return _deleteState(mst);
}

/** \brief Update payoff starting from a specified state.
 * \param st the state where the update starts
 */
void CSOSAgent::updatePayoff(State st)
{
	struct cs_State *mst = searchState(st);
	return updateStatePayoff(mst);
}

/** \brief Add Memory information to memory
 *
 * \param memif the memory information to be added.
 */
void CSOSAgent::addMemoryInfo(const struct Memory_Info *memif)
{
	UNUSED(memif);    // memory info is generated by processing, can't be directly changed.
	return;
}

/** \brief Update memory information in memory
 *
 * \param memif the memory information to be updated
 */
void CSOSAgent::updateMemoryInfo(const struct Memory_Info *memif)
{
	UNUSED(memif);
	return;
}

/** \brief Get the memory information from memory
 *
 * \return the header pointed to the memory information
 */
struct Memory_Info *CSOSAgent::getMemoryInfo() const
{
	struct Memory_Info *memif = (struct Memory_Info *) malloc(
			sizeof(struct Memory_Info));

	memif->discount_rate = discount_rate;
	memif->threshold = threshold;
	memif->state_num = state_num;
	memif->lk_num = lk_num;
	memif->last_st = pre_in;
	memif->last_act = pre_out;

	return memif;
}

/** \brief Get the name of memory
 * \return the name
 */
std::string CSOSAgent::getMemoryName() const
{
	char name[64];
	sprintf(name, "Memory_of_Agent_%d", id);
	return name;
}

/** \brief Get the first state in memory
 *
 * \return the first state value
 */
Agent::State CSOSAgent::firstState() const
{
	current_st_index = head;
	if (current_st_index != NULL)
		return current_st_index->st;
	else
		return INVALID_STATE;
}

/** \brief Get the next state in memory
 *
 * \return the state value
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

/** \brief Check if a state exists in memory
 *
 * \return true|false
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
