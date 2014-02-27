// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
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
#include <stdlib.h>
#include <assert.h>
#include "gimcs/CSMAgent.h"
#include "gimcs/Storage.h"
#include "gimcs/Debug.h"

namespace gimcs
{

CSMAgent::CSMAgent() :
        state_num(0), lk_num(0), head(NULL), cur_mst(NULL), current_st_index(
        NULL)
{
    states_map.clear();
}

CSMAgent::CSMAgent(int i) :
        MAgent(i), state_num(0), lk_num(0), head(NULL), cur_mst(NULL), current_st_index(
        NULL)
{
    states_map.clear();
}

CSMAgent::CSMAgent(int i, float dr, float th) :
        MAgent(i, dr, th), state_num(0), lk_num(0), head(NULL), cur_mst(NULL), current_st_index(
        NULL)
{
    states_map.clear();
}

CSMAgent::~CSMAgent()
{
    FreeMemory();    // free computer memory
}

/** \brief Load a specified state from a previous memory stored in database.
 *
 * \param st state value
 * \return state struct of st in computer memory
 *
 */
void CSMAgent::LoadState(Storage *storage, Agent::State st)
{
    State_Info_Header *sthd = storage->GetStateInfo(st);
    PrintStateInfo(sthd);
    if (sthd == NULL)    // should not happen, otherwise database corrupted!
        ERROR(
                "state: %ld should exist, but fetch from storage returns NULL, the database may be corrupted!\n",
                st);

    struct cs_State *mst = SearchState(st);    // search memory for the state First
    if (mst == NULL)
        AddStateInfo(sthd);
    else
        UpdateStateInfo(sthd);

    free(sthd);
    return;
}

/** \brief Initialize memory, if saved, loaded from database to computer memory, otherwise do nothing.
 *
 */

void CSMAgent::LoadMemoryFromStorage(Storage *storage)
{
    if (storage == NULL)    // no database specified, do nothing
        return;

    int re = storage->Connect();    // otherwise, load memory from database
    if (re == 0)    // successfully connected
    {
        char label[10] = "Loading: ";
        printf("Loading Memory from Storage... \n");
        fflush(stdout);

        /* load memory information */
        unsigned long saved_state_num = 0;
        struct Memory_Info *memif = storage->GetMemoryInfo();
        if (memif != NULL)
        {
            discount_rate = memif->discount_rate;
            threshold = memif->threshold;
            saved_state_num = memif->state_num;
            lk_num = memif->lk_num;
            pre_in = memif->last_st;    // it's continuous
            pre_out = memif->last_act;    //
            free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
        }

        /* load states information */
        Agent::State st = storage->FirstState();
        unsigned long progress = 0;
        while (st != INVALID_STATE)
        {
            dbgmoreprt("LoadMemory()", "LoadState: %ld\n", st);
            LoadState(storage, st);

            st = storage->NextState();
            progress++;
            PrintProcess(progress, saved_state_num, label);
        }
        // check if number of states consistent
        if (saved_state_num != state_num)
        {
            WARNNING(
                    "LoadMemory(): Number of states not consistent,which by stored meminfo says to be %ld, but in stateinfo is %ld, the storage may be conrupted!\n",
                    saved_state_num, state_num);
        }
    }
    else    // connect database failed!
    {
        WARNNING("LoadMemory(): Connecting storage failed!\n");
    }

    storage->Close();
    return;
}

/** \brief Save current memroy to database, including states information and memory-level statistics.
 *
 */
void CSMAgent::DumpMemoryToStorage(Storage *storage) const
{
    if (storage == NULL)    // no database specified, no need to save
        return;

    char label[10] = "Saving: ";
    printf("Saving Memory to Storage... \n");
    int re = storage->Connect();
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

        storage->AddMemoryInfo(memif);    // Add to storage
        free(memif);    // free it

        /* save states information */
        struct State_Info_Header *stif = NULL;
        struct cs_State *mst, *nmst;
        unsigned long index = 0;
        // walk through all state structs
        for (mst = head; mst != NULL; mst = nmst)
        {
            if (mst->flag == NEW)
            {
                dbgmoreprt("SaveMemory()", "state: %ld, Mark: %d\n", mst->st, mst->flag);
                stif = GetStateInfo(mst->st);
                assert(stif != NULL);
                storage->AddStateInfo(stif);
                free(stif);    // free
            }
            else if (mst->flag == MODIFIED)
            {
                dbgmoreprt("SaveMemory()", "state: %ld, Mark: %d\n", mst->st, mst->flag);
                stif = GetStateInfo(mst->st);
                assert(stif != NULL);
                storage->UpdateStateInfo(stif);
                free(stif);    // free
            }
            mst->flag = SAVED;    // update flag

            index++;
            PrintProcess(index, state_num, label);
            nmst = mst->next;
        }
    }
    storage->Close();
    return;
}

/** \brief search for state value in memory
 *
 * \param st state value
 * \return state struct, NULL for not exists
 *
 */

struct cs_State *CSMAgent::SearchState(Agent::State st) const
{
    StatesMap::const_iterator it = states_map.find(st);    // find the state value in hash map
    if (it != states_map.end())    // found
        return (struct cs_State *) (it->second);
    else
        return NULL;
}

/** \brief create a new state struct
 * \param st state value to be created
 * \return newly created state struct
 */
struct cs_State *CSMAgent::NewState(Agent::State st)
{
    struct cs_State *mst = (struct cs_State *) malloc(sizeof(struct cs_State));
    assert(mst != NULL);
    // fill in default values
    mst->st = st;
    mst->original_payoff = 0.0;    // any value, doesn't master, it'll be set when used. Note: this value is also used for unseen previous state recieved in links from others.
    mst->payoff = 0;
    mst->count = 1;    // it's created when we First encounter it
    mst->flag = NEW;    // it's new, and should be saved
    mst->actlist = NULL;
    mst->blist = NULL;

    mst->prev = NULL;
    mst->next = NULL;
    return mst;
}

/** \brief Free a state struct and retrieve its computer memory
 * \param mst state struct
 *
 */
void CSMAgent::FreeState(struct cs_State *mst)
{
    if (mst == NULL)
    {
        return;
    }
    // before free the struct itself, free its subparts First
    /* free actlist */
    struct cs_Action *ac, *nac;
    for (ac = mst->actlist; ac != NULL; ac = nac)
    {
        nac = ac->next;

        FreeAct(ac);
    }

    /* free blist */
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        nbas = bas->next;
        FreeBlk(bas);
    }

    /* cut off the state */
    if (mst->prev != NULL) mst->prev->next = mst->next;
    if (mst->next != NULL) mst->next->prev = mst->prev;

    return free(mst);
}

/** \brief Create a new forward arc struct
 *
 * \param eat the environment action
 * \param act the action
 * \return a new forward arc struct
 *
 */
struct cs_EnvAction *CSMAgent::NewEat(EnvAction eat)
{
    struct cs_EnvAction *meat = (struct cs_EnvAction *) malloc(
            sizeof(struct cs_EnvAction));
    meat->eat = eat;
    meat->count = 1;
    meat->nstate = NULL;

    meat->next = NULL;
    return meat;
}

/** \brief Free a forward arc struct
 *
 * \param fas the struct to be freed
 *
 */
void CSMAgent::FreeEat(struct cs_EnvAction *meat)
{
    return free(meat);
}

/** \brief Create a new Back Arc struct
 *
 * \return a new back arc struct
 *
 */
struct cs_BackwardLink *CSMAgent::NewBlk()
{
    struct cs_BackwardLink *bas = (struct cs_BackwardLink *) malloc(
            sizeof(struct cs_BackwardLink));
    bas->pstate = NULL;
    bas->next = NULL;
    return bas;
}

/** \brief Free a back arc struct
 *
 */
void CSMAgent::FreeBlk(struct cs_BackwardLink *bas)
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
struct cs_Action* CSMAgent::ActAddr(Agent::Action act,
        const struct cs_State *mst) const
{
    struct cs_Action *ac, *nac;

    // walk through action list
    for (ac = mst->actlist; ac != NULL; ac = nac)
    {
        if (ac->act == act) return ac;

        nac = ac->next;
    }

    return NULL;
}

struct cs_EnvAction *CSMAgent::EatAddrInAct(EnvAction eat, cs_Action *mact)
{
    if (mact == NULL) return NULL;

    struct cs_EnvAction *meat, *nmeat;

    for (meat = mact->ealist; meat != NULL; meat = nmeat)
    {
        if (meat->eat == eat) return meat;

        nmeat = meat->next;
    }

    return NULL;
}

void CSMAgent::AddAct2State(cs_Action *mac, cs_State *mst)
{
    mac->next = mst->actlist;
    mst->actlist = mac;
}

void CSMAgent::AddEat2Act(cs_EnvAction *meat, cs_Action *mac)
{
    meat->next = mac->ealist;
    mac->ealist = meat;
}

/** \brief Create a new action struct
 *
 * \param eat action value
 * \return a new action struct
 *
 */
struct cs_Action *CSMAgent::NewAct(Agent::Action act)
{
    struct cs_Action *ac = (struct cs_Action *) malloc(
            sizeof(struct cs_Action));

    ac->act = act;
    ac->ealist = NULL;

    ac->next = NULL;
    return ac;
}

void CSMAgent::FreeAct(struct cs_Action *ac)
{
    // free ealist
    struct cs_EnvAction *meat, *nmeat;
    for (meat = ac->ealist; meat != NULL; meat = nmeat)
    {
        nmeat = meat->next;
        FreeEat(meat);
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
void CSMAgent::LinkStates(struct cs_State *mst, EnvAction eat,
        Agent::Action act, struct cs_State *nmst)
{
    /* check if the link already exists, if so simply update the count of environment action */
    dbgmoreprt("Enter LinkStates()", "------------------------------------- Make Link: %ld == %ld + %ld => %ld\n", mst->st, eat, act, nmst->st);
    struct cs_Action *mac;
    struct cs_EnvAction *meat;

    mac = ActAddr(act, mst);
    if (mac != NULL)
    {
        meat = EatAddrInAct(eat, mac);
        if (meat != NULL)
        {
            if (meat->nstate->st != nmst->st)
            {
                ERROR(
                        "LinkStates(): act and eat are equal, but nstate diffs, this shouldn't happen!\n");
            }
            else    // link already exists
            {
                dbgmoreprt("LinkStates():", "link already exists, increase count only\n");
                meat->count++;
                return;
            }
        }
    }

    dbgmoreprt("LinkStates():", "create new link...\n");

    /* link not exists, create a new link from mst to nmst */
    if (mac == NULL)    // act not exist
    {
        mac = NewAct(act);
        AddAct2State(mac, mst);

        meat = NewEat(eat);
        meat->nstate = nmst;
        AddEat2Act(meat, mac);
    }
    else    // act exists, but eat not exist, meat == NULL
    {
        meat = NewEat(eat);
        meat->nstate = nmst;
        AddEat2Act(meat, mac);
    }

    /* Check and add mst to nmst's blist */
    // state shouldn't repeat in backward list, check if already exists
    struct cs_BackwardLink *bas, *nbas;
    for (bas = nmst->blist; bas != NULL; bas = nbas)
    {
        if (bas->pstate == mst)    // found
            break;

        nbas = bas->next;
    }

    if (bas == NULL)    // not found, create a new one and Add to blist
    {
        bas = NewBlk();
        bas->pstate = mst;    // previous state is mst
        // Add bas to nmst's blist
        bas->next = nmst->blist;
        nmst->blist = bas;
    }
    else    // found ,nothing to do
    {
    }

    lk_num++;    // update total link number
    return;
}

/**
 * \brief Calculate the possibility of encountering an specified environment action
 * \param ea environment
 * \param mst the state struct
 * \return the possibility
 */
float CSMAgent::Prob(const struct cs_EnvAction *ea,
        const struct cs_Action *mac) const
{
    unsigned long eacount = ea->count;
    // calculate the sum of env action counts
    unsigned long sum_eacount = 0;
    struct cs_EnvAction *pea, *pnea;
    for (pea = mac->ealist; pea != NULL; pea = pnea)
    {
        sum_eacount += pea->count;
//        dbgprt("Prob", "eat: %ld, count %ld, ", pea->eat, pea->count);

        pnea = pea->next;
    }

    // state count donesn't equal to sum of eacount due to the set operation (actually state count will become smaller than sum eacount gradually)
    dbgmoreprt("Prob", "------- action: %ld\n", mac->act);dbgmoreprt("Prob", "sum: %ld\n", sum_eacount);

    float re = (1.0 / sum_eacount) * eacount;    // number of env actions divided by the total number
    /* do some checks below */
    // check if re is in range (0, 1]
    if (re < 0 || re > 1)    // check failed
    {
        ERROR(
                "Prob(): probability is %.2f, which must in range [0, 1]. action: %ld, eact is %ld, count is %ld, total eacount is %ld.\n",
                re, mac->act, ea->eat, eacount, sum_eacount);
    }

    return re;
}

/**
 * \brief Calculate payoff of the specified state
 * \param mst specified state
 * \return payoff of the state
 */
float CSMAgent::CalStatePayoff(const struct cs_State *mst) const
{
    dbgmoreprt("\nCalStatePayoff()", "-------------------------------- state: %ld, count: %ld\n", mst->st, mst->count);

    float u0 = mst->original_payoff;
    float pf = u0;    // set initial value as original payoff
    float tmp = 0.0;

    struct cs_Action *mac, *nmac;
    struct cs_EnvAction *ea, *nea;
    for (mac = mst->actlist; mac != NULL; mac = nmac)
    {
        for (ea = mac->ealist; ea != NULL; ea = nea)
        {
            tmp += Prob(ea, mac) * ea->nstate->payoff;

            nea = ea->next;
        }
        nmac = mac->next;
    }
    pf += discount_rate * tmp;

    return pf;
}

/**
 * \brief Update states backwards recursively beginning from a specified state
 * Note that: every time a state makes any changes, all its previous states must be updated!
 * \param mst a specified state where the update begins
 */
void CSMAgent::UpdateStatePayoff(struct cs_State *mst)
{
    /* update state's payoff recursively */
    float payoff = CalStatePayoff(mst);
    dbgmoreprt("\nUpdateState()", "------------------------------------------- state: %ld, intial payoff:%.1f to %.1f\n", mst->st, mst->payoff, payoff);

    if (fabsf(mst->payoff - payoff) >= threshold)    // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff = payoff;
        dbgmoreprt("UpdateState()", "Change to payoff: %.1f\n", payoff);
        if (mst->flag == SAVED) mst->flag = MODIFIED;    // set flag to indicate the modification, no need to change if it's NEW

        /* update backwards recursively */
        struct cs_BackwardLink *bas, *nbas;
        for (bas = mst->blist; bas != NULL; bas = nbas)
        {
            UpdateStatePayoff(bas->pstate);    // recursively update
            nbas = bas->next;
        }
    }
    else
    {
        dbgmoreprt("UpdateState()", "Payoff no changes, it's smaller than %.1f\n", threshold);
    }

    return;
}

/**
 * \brief Calculate payoff of a specified action of a state
 * \param act action value
 * \param mst state struct which contains the action
 * \return payoff of the action
 */
float CSMAgent::CalActPayoff(Agent::Action act,
        const struct cs_State *mst) const
{
    float payoff = 0;

    struct cs_EnvAction *ea, *nea;
    struct cs_Action *mac = ActAddr(act, mst);
    if (mac == NULL)    // this is an unseen action
        return degree_of_curiosity;

    for (ea = mac->ealist; ea != NULL; ea = nea)
    {
        payoff += Prob(ea, mac) * ea->nstate->payoff;

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
OSpace CSMAgent::BestActions(const struct cs_State *mst, OSpace &acts) const
{
    float max_payoff = -FLT_MAX;
    float payoff;
    OSpace max_acts;

    max_acts.Clear();
    // walk through every action in list
    Agent::Action act = acts.First();
    while (act != INVALID_OUTPUT)    // until out of bound
    {
        payoff = CalActPayoff(act, mst);

        if (payoff > max_payoff)    // find a bigger one, refill the max payoff action list
        {
            max_acts.Clear();
            max_acts.Add(act);
            max_payoff = payoff;
        }
        else if (payoff == max_payoff)    // find an equal one, Add it to the list
            max_acts.Add(act);

        act = acts.Next();
    }
    return max_acts;
}

/**
 * \brief Update states in memory. Note: This function should be called AFTER MaxPayoffRule() in every step!
 * \param oripayoff original payoff of current state
 */
void CSMAgent::UpdateMemory(float oripayoff)
{
    dbgmoreprt("\nEnter UpdateMemory()", "-------------------------------------------------\n");
    if (pre_in == INVALID_STATE)    // previous state not exist, it's running for the First time
    {
        dbgmoreprt("", "Previous state not exists, create current state in memory.\n");
        //NOTE: cur_mst is already set in MaxPayoffRule() function
        if (cur_mst == NULL)    // create current state in memory
        {
            cur_mst = NewState(cur_in);
            cur_mst->original_payoff = oripayoff;    // set original payoff as given
            cur_mst->payoff = oripayoff;    // set payoff as original payoff

            // Add current state to memory
            AddStateToMemory(cur_mst);
        }
        else    // state found, this could happen if others send state information to me before the First time I'm running
        {
            dbgmoreprt("", "Previous state not exists, but I recieved some information of this state from others.\n");
            // update current state
            cur_mst->count++;    // inc state count
            cur_mst->original_payoff = oripayoff;    // reset original payoff
            // no previous state, so no link involved
        }

        return;
    }

    dbgmoreprt("", "Previous state is %ld.\n", pre_in);
    /* previous state exists */
    struct cs_State *pmst = SearchState(pre_in);    // found previous state struct
    if (pmst == NULL)
        ERROR(
                "UpdateMemory(): Can not find previous state in memory, which should be existing!\n");

    //NOTE: cur_mst is already set in MaxPayoffRule() function
    if (cur_mst == NULL)    // currrent state struct not exists in memory, create it in memory, and link it to the previous state
    {
        dbgmoreprt("", "current state not exists, create it and build the link\n");
        cur_mst = NewState(cur_in);
        cur_mst->original_payoff = oripayoff;
        cur_mst->payoff = oripayoff;
        // Add it to memory
        AddStateToMemory(cur_mst);

        // build the link
        EnvAction peat = cur_in - pre_in - pre_out;    // calcuate previous environment action. This formula is important!!!
        LinkStates(pmst, peat, pre_out, cur_mst);    // build the link
    }
    else    // current state struct already exists, update the count and link it to the previous state (LinkStates will handle it if the link already exists.)
    {
        // test
        State_Info_Header *tmphd = GetStateInfo(cur_in);
        PrintStateInfo(tmphd);
        free(tmphd);

        dbgmoreprt("", "current state is %ld, increase count and build the link\n", cur_mst->st);
        // update current state
        cur_mst->count++;    // inc state count
        cur_mst->original_payoff = oripayoff;    // reset original payoff

        // build the link
        EnvAction peat = cur_in - pre_in - pre_out;
        LinkStates(pmst, peat, pre_out, cur_mst);
    }

    UpdateStatePayoff(cur_mst);    // update states recursively
    return;
}

/**
 * \brief Free computer memory used by the agent's memory.
 */
void CSMAgent::FreeMemory()
{
    // free all states in turn
    struct cs_State *mst, *nmst;
    for (mst = head; mst != NULL; mst = nmst)
    {
        nmst = mst->next;
        FreeState(mst);
    }
    states_map.clear();
}

/**
 * \brief Remove and free a specified state from agent's memory.
 *
 * \param mst the state to be removed
 * FIXME: need to handle the links with other states!
 */
void CSMAgent::DeleteState(struct cs_State *mst)
{
    WARNNING(
            "DeleteState() is not completely implemented yet, it's buggy and will not work as expected, DON'T use it!\n");

    if (mst == NULL)
    {
        return;
    }

    /* TODO: firstly remove forward and backward links with other states */

    // free the state itself
    return FreeState(mst);
}

/**
 * \brief Implementation of the Maximun Payoff Rule (MPR).
 * \param st state which is concerned
 * \param acts the candidate action list
 * \return actions choosen by MPR
 */
OSpace CSMAgent::MaxPayoffRule(Agent::State st, OSpace &acts) const
{
    dbgmoreprt("Enter MaxPayoffRule() ", "-------------------------------------------------- State: %ld\n", st);
    cur_mst = SearchState(st);    // get the state struct from state value
    OSpace re;

    if (cur_mst == NULL)    // First time to encounter this state, we know nothing about it, so no restriction applied, return the whole list
    {
        dbgmoreprt("MaxPayoffRule()", "State not found in memory.\n");
        re = acts;
    }
    else    // we have memories about this state, find the best action of it
    {
        re = BestActions(cur_mst, acts);
    }

    return re;
}

/**
 * \brief Get information of specified state from memory
 * \param st the state whose information is requested
 * \return header pointed to the state information, NULL for error
 */
struct State_Info_Header *CSMAgent::GetStateInfo(Agent::State st) const
{
    if (st == INVALID_STATE)    // check if valid
    {
        dbgmoreprt("GetStateInfo()", "invalid state value\n");
        return NULL;
    }

    struct cs_State *mst;
    mst = SearchState(st);    // find the state struct in computer memory

    if (mst == NULL)    // error, not found
    {
        dbgmoreprt("GetStateInfo()", "state: %ld not found in memory!\n", st);
        return NULL;
    }

    // get the total size of state
    int sthd_size = sizeof(State_Info_Header);
    int act_num = 0;
    struct cs_Action *mac, *nmac;
    struct cs_EnvAction *ea, *nea;
    for (mac = mst->actlist; mac != NULL; mac = nmac)
    {
        sthd_size += sizeof(Action_Info_Header);
        for (ea = mac->ealist; ea != NULL; ea = nea)
        {
            sthd_size += sizeof(EnvAction_Info);

            nea = ea->next;
        }

        act_num++;
        nmac = mac->next;
    }

    State_Info_Header *sthd = (State_Info_Header *) malloc(sthd_size);
    // fill the header
    sthd->st = st;
    sthd->original_payoff = mst->original_payoff;
    sthd->payoff = mst->payoff;
    sthd->count = mst->count;
    sthd->act_num = act_num;
    sthd->size = sthd_size;

    unsigned char *stp = (unsigned char *) sthd;    // use point stp to travel through each subpart of state
    unsigned char *atp;
    stp += sizeof(State_Info_Header);    // point to the first act
    for (mac = mst->actlist; mac != NULL; mac = nmac)
    {
        Action_Info_Header *athd = (Action_Info_Header *) stp;
        athd->act = mac->act;

        atp = stp;
        atp += sizeof(Action_Info_Header);    // point to the first eat of act
        int ea_num = 0;
        for (ea = mac->ealist; ea != NULL; ea = nea)
        {
            EnvAction_Info *eaif = (EnvAction_Info *) atp;
            eaif->eat = ea->eat;
            eaif->count = ea->count;
            eaif->nst = ea->nstate->st;

            ea_num++;
            atp += sizeof(EnvAction_Info);    // point to the next eat
            nea = ea->next;
        }

        athd->eat_num = ea_num;

        stp += sizeof(Action_Info_Header)
                + athd->eat_num * sizeof(EnvAction_Info);    // point to the next act
        nmac = mac->next;
    }

    return sthd;
}

void CSMAgent::AddStateInfo(const State_Info_Header *sthd)
{
    if (sthd == NULL) return;

#ifdef _DEBUG_MORE_
    printf("---------------------- AddStateInfo: ---------------------");
    PrintStateInfo(sthd);
#endif

    struct cs_State *mst = SearchState(sthd->st);    // search for the state
    if (mst != NULL)    // state already exists, use UpdateStateInfo() instead!
    {
        WARNNING(
                "AddStateInfo(): state %ld already exists in memory, if you want to change it, using UpdateStateInfo()!\n",
                sthd->st);
        return;
    }

    dbgmoreprt("AddStateInfo()",
            "state: %ld, create it in memory.\n", sthd->st);

    mst = NewState(sthd->st);
    // copy state information
    mst->count = sthd->count;
    mst->payoff = sthd->payoff;
    mst->original_payoff = sthd->original_payoff;    // the original payoff is what really is important
    /* Add to memory */
    AddStateToMemory(mst);

    // copy actlist
    unsigned char *stp = (unsigned char *) sthd;    // use point stp to travel through each subpart of state
    unsigned char *atp;
    // environment action information
    stp += sizeof(struct State_Info_Header);    // point to the first act
    int anum;
    for (anum = 0; anum < sthd->act_num; anum++)
    {
        Action_Info_Header *athd = (Action_Info_Header *) stp;
        // create this act and add it to state
        cs_Action *mac = NewAct(athd->act);
        AddAct2State(mac, mst);

        atp = stp;
        atp += sizeof(Action_Info_Header);    // point to the first eat of act
        int i;
        for (i = 0; i < athd->eat_num; i++)    // copy every eat of this act
        {
            EnvAction_Info *eaif = (EnvAction_Info *) atp;
            // create this eat and add it to act
            cs_EnvAction *meat = NewEat(eaif->eat);
            meat->count = eaif->count;
            // links to the next state
            Agent::State nst = eaif->nst;    // Next state value
            struct cs_State *nmst = SearchState(nst);    // find if the Next state exists
            if (nmst != NULL)    // if so, inc count and make the link
            {
                // build the link
                dbgmoreprt("next state", "%ld exists, build the link\n", nst);
                meat->nstate = nmst;
            }
            else    // for a non-existing Next state, we will create it, and build the link
            {
                // create a new previous state
                dbgmoreprt("next state", "%ld not exists, create it and build the link\n", nst);
                struct cs_State *nmst = NewState(nst);
                // Add to memory
                AddStateToMemory(nmst);
                // build the link
                meat->nstate = nmst;
            }

            AddEat2Act(meat, mac);
            atp += sizeof(EnvAction_Info);    // point to the next eat
        }

        stp += sizeof(Action_Info_Header)
                + athd->eat_num * sizeof(EnvAction_Info);    // point to the next act
    }

    /* UpdatePayoff starting from mst's previous states. This update is IMPORTANT!
     * The information of mst need to be merged to memory by update, otherwise others parts of the memory will know
     * nothing about mst, then the information we got is useless.
     * */
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        UpdateStatePayoff(bas->pstate);    // update previous state one by one

        nbas = bas->next;
    }

    return;
}

void CSMAgent::UpdateStateInfo(const State_Info_Header *sthd)
{
    if (sthd == NULL) return;

#ifdef _DEBUG_MORE_
    printf("---------------------- UpdateStateInfo: ---------------------");
    PrintStateInfo(sthd);
#endif

    struct cs_State *mst = SearchState(sthd->st);    // search for the state
    if (mst == NULL)    // state doesn't exists, use AddStateInfo() instead!
    {
        WARNNING(
                "UpdateStateInfo(): state %ld doesn't exist in memory, if you want to add it, using AddStateInfo()!\n",
                sthd->st);
        return;
    }

    dbgmoreprt("UpdateStateInfo()", "update information of state %ld.\n", sthd->st);

    // free old stuff
    // free actlist
    struct cs_Action *mac, *nmac;
    for (mac = mst->actlist; mac != NULL; mac = nmac)
    {
        nmac = mac->next;
        FreeAct(mac);
    }
    mst->actlist = NULL;    // set as NULL! It's very important!

    // create and copy as AddStateInfo()
    // copy state information
    mst->count = sthd->count;
    mst->payoff = sthd->payoff;
    mst->original_payoff = sthd->original_payoff;    // the original payoff is what really is important

    // copy actlist
    unsigned char *stp = (unsigned char *) sthd;    // use point stp to travel through each subpart of state
    unsigned char *atp;
    stp += sizeof(struct State_Info_Header);    // point to the first act
    int anum;
    // environment action information
    for (anum = 0; anum < sthd->act_num; anum++)
    {
        Action_Info_Header *athd = (Action_Info_Header *) stp;
        // create this act and add it to state
        cs_Action *mac = NewAct(athd->act);
        AddAct2State(mac, mst);

        atp = stp;
        atp += sizeof(Action_Info_Header);    // point to the first eat of act
        int i;
        for (i = 0; i < athd->eat_num; i++)    // copy every eat of this act
        {
            EnvAction_Info *eaif = (EnvAction_Info *) atp;
            // create this eat and add it to act
            cs_EnvAction *meat = NewEat(eaif->eat);
            meat->count = eaif->count;
            // links to the next state
            Agent::State nst = eaif->nst;    // Next state value
            struct cs_State *nmst = SearchState(nst);    // find if the Next state exists
            if (nmst != NULL)    // if so, inc count and make the link
            {
                // build the link
                dbgmoreprt("next state", "%ld exists, build the link\n", nst);
                meat->nstate = nmst;
            }
            else    // for a non-existing Next state, we will create it, and build the link
            {
                // create a new previous state
                dbgmoreprt("next state", "%ld not exists, create it and build the link\n", nst);
                struct cs_State *nmst = NewState(nst);
                // Add to memory
                AddStateToMemory(nmst);
                // build the link
                meat->nstate = nmst;
            }

            AddEat2Act(meat, mac);
            atp += sizeof(EnvAction_Info);    // point to the next eat
        }

        stp += sizeof(Action_Info_Header)
                + athd->eat_num * sizeof(EnvAction_Info);    // point to the next act
    }

    /* UpdatePayoff starting from mst's previous states. This update is IMPORTANT!
     * The information of mst need to be merged to memory by update, otherwise others parts of the memory will know
     * nothing about mst, then the information we got is useless.
     * */
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        UpdateStatePayoff(bas->pstate);    // update previous state one by one

        nbas = bas->next;
    }

    return;
}

void CSMAgent::DeleteState(State st)
{
    struct cs_State *mst = SearchState(st);
    return DeleteState(mst);
}

void CSMAgent::UpdatePayoff(State st)
{
    struct cs_State *mst = SearchState(st);
    return UpdateStatePayoff(mst);
}

Agent::State CSMAgent::FirstState() const
{
    current_st_index = head;
    if (current_st_index != NULL)
        return current_st_index->st;
    else
        return INVALID_STATE;
}

Agent::State CSMAgent::NextState() const
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

bool CSMAgent::HasState(State st) const
{
    struct cs_State *mst = SearchState(st);
    if (mst == NULL)
        return false;
    else
        return true;
}

/**
 * \brief Pretty print process of load or save memory to database.
 * \param current current progress
 * \param total total amount
 * \param label indicator label
 */
void CSMAgent::PrintProcess(unsigned long current, unsigned long total,
        char *label) const
{
    double prcnt;
    int num_of_dots;
    char buffer[80] = { 0 };
    int width;
    /* get term width */
    FILE *fp;
    prcnt = 1.0 * current / total;
    fp = popen("stty size | cut -d\" \" -f2", "r");
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    width = atoi(buffer);

    if (width < 32)
    {
        printf("\e[1A%3d%% completed.\n", (int) (prcnt * 100));
    }
    else
    {
        num_of_dots = width - 20;

        char *pline_to_print = (char *) malloc(sizeof(char) * width);
        int dots = (int) (num_of_dots * prcnt);

        memset(pline_to_print, 0, width);
        memset(pline_to_print, '>', dots);
        memset(pline_to_print + dots, ' ', num_of_dots - dots);
        printf("\e[1A%s[%s] %3d%% \n", label, pline_to_print,
                (int) (prcnt * 100));
        free(pline_to_print);
    }
    return;
}

void CSMAgent::AddStateToMemory(struct cs_State *nstate)
{
    // Add nstate to the front
    nstate->prev = NULL;
    nstate->next = head;
    if (head != NULL) head->prev = nstate;
    head = nstate;

    state_num++;
    states_map.insert(StatesMap::value_type(nstate->st, nstate));    // don't forget to update hash map
}

}    // namespace gimcs
