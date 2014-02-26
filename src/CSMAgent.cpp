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
    struct cs_State *mst = SearchState(st);    // search memory for the state First
    if (mst == NULL)    // not found, create a new empty state struct, this struct will be filled up below
    {
        mst = NewState(st);
        /* Add to memory */
        AddStateToMemory(mst);
    }

    struct State_Info_Header *stif = NULL;
    stif = storage->GetStateInfo(st);    // get state information from database, return the size
    if (stif == NULL)    // should not happen, otherwise database corrupted!
        ERROR(
                "state: %ld should exist, but fetch from storage returns NULL, the database may be corrupted!\n",
                st);

    mst->flag = SAVED;    // it's SAVED when just loaded
    mst->st = stif->st;
    mst->original_payoff = stif->original_payoff;
    mst->payoff = stif->payoff;
    mst->count = stif->count;

    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);
    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);

    unsigned char *p = (unsigned char *) stif;    // use point p to travel through even parts
    // point to env actions
    p += sizeof(struct State_Info_Header);
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // point to actions
    p += ea_len;
    struct Action_Info *atif = (struct Action_Info *) p;

    // point to forward links
    p += ai_len;
    struct Forward_Link_Info *lk = (struct Forward_Link_Info *) p;

    int i;
    /* build exactions list */
    for (i = 0; i < stif->eat_num; i++)
    {
        struct cs_EnvAction *mea = NewEa(eaif[i].eat);
        mea->count = eaif[i].count;
        // Add to mst's ealist
        mea->next = mst->ealist;
        mst->ealist = mea;
    }

    /* build actions list */
    for (i = 0; i < stif->act_num; i++)
    {
        struct cs_Action *mac = NewAc(atif[i].act);
        mac->payoff = atif[i].payoff;
        // Add to mst's atlist
        mac->next = mst->atlist;
        mst->atlist = mac;
    }

    /* build current state's forward list and Next states' backward list */
    for (i = 0; i < stif->lk_num; i++)
    {
        struct cs_State *nmst = SearchState(lk[i].nst);    // search for Next state in memory
        if (nmst == NULL)    // Next state not found, create one
        {
            nmst = NewState(lk[i].nst);
            // Add to memory
            AddStateToMemory(nmst);
        }

        // build mst's forward list
        struct cs_ForwardLink *mfas = NewFlk(lk[i].eat, lk[i].act);
        mfas->nstate = nmst;
        // Add to mst's flist
        mfas->next = mst->flist;
        mst->flist = mfas;

        // build nmst's backward list
        struct cs_BackwardLink *mbas = NewBlk();
        mbas->pstate = mst;
        // Add to nmst's blist
        mbas->next = nmst->blist;
        nmst->blist = mbas;
    }

    free(stif);    // dont't forget to free

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
    // before free the struct itself, free its subparts First
    /* free actlist */
    struct cs_Action *ac, *nac;
    for (ac = mst->actlist; ac != NULL; ac = nac)
    {
        nac = ac->next;

        FreeAc(ac);
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
struct cs_ForwardLink *CSMAgent::NewFlk(EnvAction eat)
{
    struct cs_ForwardLink *fas = (struct cs_ForwardLink *) malloc(
            sizeof(struct cs_ForwardLink));
    fas->eat = eat;
    fas->count = 1;
    fas->nstate = NULL;

    fas->next = NULL;
    return fas;
}

/** \brief Free a forward arc struct
 *
 * \param fas the struct to be freed
 *
 */
void CSMAgent::FreeFlk(struct cs_ForwardLink *fas)
{
    return free(fas);
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
struct cs_Action* CSMAgent::AddrOfAct(Agent::Action act,
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

/** \brief Create a new action struct
 *
 * \param eat action value
 * \return a new action struct
 *
 */
struct cs_Action *CSMAgent::NewAc(Agent::Action act)
{
    struct cs_Action *ac = (struct cs_Action *) malloc(
            sizeof(struct cs_Action));

    ac->act = act;
    ac->flist = NULL;

    ac->next = NULL;
    return ac;
}

void CSMAgent::FreeAc(struct cs_Action *ac)
{
    // free flist
    struct cs_ForwardLink *flk, *nflk;
    for (flk = ac->flist; flk != NULL; flk = nflk)
    {
        nflk = flk->next;
        FreeFlk(flk);
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
void CSMAgent::LinkStates(struct cs_State *pmst, EnvAction eat,
        Agent::Action act, struct cs_State *mst)
{
    /* check if the link already exists, if so simply update the count of environment action */
    dbgmoreprt("Enter LinkStates()", "------------------------------------- Make Link: %ld == %ld + %ld => %ld\n", pmst->st, eat, act, mst->st);
    struct cs_Action *ac, *nac;
    struct cs_ForwardLink *flk, *nflk;

    ac = AddrOfAct(act, pmst);
    if (ac != NULL)
    {
        for (flk = ac->flist; flk != NULL; flk = nflk)
        {
            if (flk == eat && flk->nstate->st == mst->st)    // link already exists
            {
                dbgmoreprt("", "link already exists, increase count only\n");
                flk->count++;
                return;    // done, return
            }

            nflk = flk->next;
        }
    }

    dbgmoreprt("", "create new link...\n");
    /* link not exists, create a new link from pmst to mst */
    /* Add mst to pmst's flist */
    struct cs_ForwardLink *fas = NewFlk(eat, act);
    fas->nstate = mst;    // Next state is mst
    // Add fas to pmst's flist
    fas->next = pmst->flist;
    pmst->flist = fas;

    /* Add pmst to mst's blist */
    // state shouldn't repeat in backward list, check if already exists
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        if (bas->pstate == pmst)    // found
            break;

        nbas = bas->next;
    }

    if (bas == NULL)    // not found, create a new one and Add to blist
    {
        bas = NewBlk();
        bas->pstate = pmst;    // previous state is pmst
        // Add bas to mst's blist
        bas->next = mst->blist;
        mst->blist = bas;
    }
    else    // found ,nothing to do
    {
    }

    lk_num++;    // update total link number
    /* A link is a combination of eaction AND aciton, either of them not existing means this link doesn't exist.
     *  So we have to figure out exactly which of them doesn't exist.
     */
    /* check eaction */
    struct cs_EnvAction *ea = Eat2Struct(eat, pmst);

    if (ea == NULL)    // eaction not exist, Add a new one to the ealist of pmst
    {
        struct cs_EnvAction *nea = NewEa(eat);
        // Add nea to pmst's environment action list
        nea->next = pmst->ealist;
        pmst->ealist = nea;
    }
    else
    {
        // update count if exists
        ea->count++;    // inc count
    }

    /* check action */
    struct cs_Action *ac = AddrOfAct(act, pmst);
    if (ac == NULL)    // action not exist, Add a new one to atlist of pmst
    {
        struct cs_Action *nac = NewAc(act);
        // Add nac to pmst's action list
        nac->next = pmst->atlist;
        pmst->atlist = nac;
    }
    return;
}

/**
 * \brief Find the maximum payoff of states linked in a specified environment action to one state.
 * This function to env action is like CalActPayoff to action and CalStatePayoff to state.
 * \param eat specified environment action
 * \param mst to which state the link is
 * \return the maximum payoff
 */
float CSMAgent::MaxPayoffInEat(EnvAction eat, const struct cs_State *mst) const
{
    dbgmoreprt("Enter MaxPayoffEat()", "-------------- eat: %ld, state: %ld\n", eat, mst->st);
    float max_pf = -FLT_MAX;    // set to a possibly minimun value
    struct cs_State *nmst;
    struct cs_ForwardLink *fas, *nfas;

    bool eat_found_in_flist = false;    // indicate if an env action in ealist was found on forward list, it should be consistent!

    // walk through the forward links
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        dbgmoreprt("forward links", "eat: %ld, act: %ld, nstate: %ld\n", fas->eat, fas->act, fas->nstate->st);
        if (fas->eat == eat)
        {
            eat_found_in_flist = true;    // env action found on flist

            nmst = fas->nstate;
            dbgmoreprt("next state", "%ld, payoff: %.1f\n", nmst->st, nmst->payoff);

            if (nmst->payoff > max_pf)    // record the bigger one
                max_pf = nmst->payoff;
        }

        nfas = fas->next;
    }

    if (eat_found_in_flist == false)
    {
        ERROR(
                "MaxPayoffInEat(): env action: %ld is in ealist but not found in flist!\n",
                eat);
    }

    return max_pf;
}

/**
 * \brief Calculate the possibility of encountering an specified environment action
 * \param ea environment
 * \param mst the state struct
 * \return the possibility
 */
float CSMAgent::Prob(const struct cs_EnvAction *ea,
        const struct cs_State *mst) const
{
    unsigned long eacount = ea->count;
    // calculate the sum of env action counts
    unsigned long sum_eacount = 0;
    struct cs_EnvAction *pea, *pnea;
    for (pea = mst->ealist; pea != NULL; pea = pnea)
    {
        sum_eacount += pea->count;
//        dbgprt("Prob", "eat: %ld, count %ld, ", pea->eat, pea->count);

        pnea = pea->next;
    }

    // state count donesn't equal to sum of eacount due to the set operation (actually state count will become smaller than sum eacount gradually)
    dbgmoreprt("Prob", "------- state: %ld, count %ld, ", mst->st, mst->count);dbgmoreprt("Prob", "sum: %ld\n", sum_eacount);

    float re = (1.0 / sum_eacount) * eacount;    // number of env actions divided by the total number
    /* do some checks below */
    // check if re is in range (0, 1]
    if (re < 0 || re > 1)    // check failed
    {
        ERROR(
                "Prob(): probability is %.2f, which must in range [0, 1]. state: %ld, eact is %ld, count is %ld, total eacount is %ld.\n",
                re, mst->st, ea->eat, eacount, sum_eacount);
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
    float tmp;

    struct cs_EnvAction *ea, *nea;

    // walk through all environment actions
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        dbgmoreprt("Ealist", "=======================================\n");dbgmoreprt("", "eid: %ld, count: %ld\n", ea->eat, ea->count);
        tmp = Prob(ea, mst) * MaxPayoffInEat(ea->eat, mst);
        dbgmoreprt("", "tmp: %.1f, prob: %.1f, maxpayoffineat: %.1f\n", tmp, Prob(ea, mst), MaxPayoffInEat(ea->eat, mst));
        pf += tmp * discount_rate;    // accumulative total
        nea = ea->next;
    }dbgmoreprt("Ealist End", "==================================\n");

    return pf;
}

/**
 * \brief Update states backwards recursively beginning from a specified state
 * Note that: every time a state makes any changes, all its previous states must be updated!
 * \param mst a specified state where the update begins
 */
void CSMAgent::UpdateState(struct cs_State *mst)
{

    /* update actions' payoff */
    struct cs_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        ac->payoff = CalActPayoff(ac->act, mst);
        nac = ac->next;
    }

    /* update state's payoff recursively */
    float payoff = CalStatePayoff(mst);
    dbgmoreprt("\nUpdateState()", "------------------------------------------- state: %ld, intial payoff:%.1f to %.1f\n", mst->st, mst->payoff, payoff);

    if (fabsf(mst->payoff - payoff) >= threshold)    // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff = payoff;
        dbgmoreprt("UpdateState()", "Change to payoff: %.1f\n", payoff);

        /* update backwards recursively */
        struct cs_BackwardLink *bas, *nbas;
        for (bas = mst->blist; bas != NULL; bas = nbas)
        {
            UpdateState(bas->pstate);    // recursively update
            nbas = bas->next;
        }
    }
    else
    {
        dbgmoreprt("UpdateState()", "Payoff no changes, it's smaller than %.1f\n", threshold);
    }

    if (mst->flag == SAVED) mst->flag = MODIFIED;    // set flag to indicate the modification, no need to change if it's NEW
    return;
}

/**
 * \brief Return the state which is linked by a specified with specified environment action and action.
 * \param eat environment action value
 * \param act action value
 * \param mst the state which links the needed state
 * \return state struct needed, NULL if not found
 */
struct cs_State *CSMAgent::StateByEatAct(EnvAction eat, Agent::Action act,
        const struct cs_State *mst) const
{
    struct cs_ForwardLink *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        if (fas->eat == eat && fas->act == act)    // check both envir action and action value
        return fas->nstate;
        nfas = fas->next;
    }
    return NULL;
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
    struct cs_State *nmst;

    // walk through the envir action list
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        nmst = StateByEatAct(ea->eat, act, mst);    // find the states which are reached by performing this action
        if (nmst != NULL)
        {
            payoff += Prob(ea, mst) * (nmst->payoff);    // accumulative total
        }
        nea = ea->next;
    }dbgmoreprt("CalActPayoff()", "state: %ld, act: %ld, payoff:%.1f\n", mst->st, act, payoff);
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
        struct cs_Action *mac = AddrOfAct(act, mst);    // get action struct from values

        if (mac != NULL)
            payoff = mac->payoff;
        else
            // this is an unseen action
            payoff = degree_of_curiosity;

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
        dbgmoreprt("", "current state is %ld, increase count and build the link\n", cur_mst->st);
        // update current state
        cur_mst->count++;    // inc state count
        cur_mst->original_payoff = oripayoff;    // reset original payoff

        // build the link
        EnvAction peat = cur_in - pre_in - pre_out;
        LinkStates(pmst, peat, pre_out, cur_mst);
    }

    UpdateState(cur_mst);    // update states recursively
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

    /* then free all data of state */
    // free flist
    struct cs_ForwardLink *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        nfas = fas->next;
        FreeFlk(fas);
    }

    // free ealist
    struct cs_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        nea = ea->next;
        FreeEa(ea);
    }

    // free atlist
    struct cs_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        nac = ac->next;
        FreeAc(ac);
    }

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

    /* numbers */
    int eat_num = 0;    // number of envir actions
    int act_num = 0;    // number of actions
    int lk_num = 0;    // number of forward links

    /* get env action numbers */
    struct cs_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        eat_num++;
        nea = ea->next;
    }
    /* get action numbers */
    struct cs_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        act_num++;
        nac = ac->next;
    }
    /* get forward link numbers */
    struct cs_ForwardLink *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        lk_num++;
        nfas = fas->next;
    }

    // allocate memory
    int stif_size = sizeof(struct State_Info_Header)
            + eat_num * sizeof(struct EnvAction_Info)
            + act_num * sizeof(struct Action_Info)
            + lk_num * sizeof(struct Forward_Link_Info);

    struct State_Info_Header *stif = (struct State_Info_Header *) malloc(
            stif_size);

    // fill the header
    stif->st = st;
    stif->original_payoff = mst->original_payoff;
    stif->payoff = mst->payoff;
    stif->count = mst->count;
    stif->eat_num = eat_num;
    stif->act_num = act_num;
    stif->lk_num = lk_num;
    stif->size = stif_size;

    unsigned char *ptr = (unsigned char *) stif;    // use point ptr to travel through subparts of state information

    /* fill env action information */
    ptr += sizeof(struct State_Info_Header);
    struct EnvAction_Info eaif;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        eaif.count = ea->count;
        eaif.eat = ea->eat;
        memcpy(ptr, &eaif, sizeof(struct EnvAction_Info));
        ptr += sizeof(struct EnvAction_Info);

        nea = ea->next;
    }
    /* fill action information */
    struct Action_Info acif;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        acif.act = ac->act;
        acif.payoff = ac->payoff;
        memcpy(ptr, &acif, sizeof(struct Action_Info));
        ptr += sizeof(struct Action_Info);

        nac = ac->next;
    }

    /* forward link information */
    struct Forward_Link_Info flk;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        flk.act = fas->act;
        flk.eat = fas->eat;
        flk.nst = fas->nstate->st;
        memcpy(ptr, &flk, sizeof(struct Forward_Link_Info));
        ptr += sizeof(struct Forward_Link_Info);

        nfas = fas->next;
    }

    if ((ptr - (unsigned char *) stif) != stif_size)    // check size
    {
        ERROR("GetStateInfo(): state information header size not match!\n");
    }

    return stif;
}

void CSMAgent::AddStateInfo(const State_Info_Header *stif)
{
    if (stif == NULL) return;

#ifdef _DEBUG_MORE_
    printf("---------------------- AddStateInfo: ---------------------");
    PrintStateInfo(stif);
#endif

    struct cs_State *mst = SearchState(stif->st);    // search for the state
    if (mst != NULL)    // state already exists, use UpdateStateInfo() instead!
    {
        WARNNING(
                "AddStateInfo(): state %ld already exists in memory, if you want to change it, using UpdateStateInfo()!\n",
                stif->st);
        return;
    }

    dbgmoreprt("AddStateInfo()",
            "state: %ld, create it in memory.\n", stif->st);

    unsigned char *p = (unsigned char *) stif;    // use point p to travel through each subpart
    // environment action information
    p += sizeof(struct State_Info_Header);
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // action information
    int len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct Action_Info *atif = (struct Action_Info *) p;

    // forward links information
    len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct Forward_Link_Info *flk = (struct Forward_Link_Info *) p;

    mst = NewState(stif->st);
    // copy state information
    mst->count = stif->count;    // copy count
    mst->payoff = stif->payoff;
    mst->original_payoff = stif->original_payoff;    // the original payoff is what really is important

    /* Add to memory */
    AddStateToMemory(mst);
    int i;
    /* create and copy ExActions information */
    for (i = 0; i < stif->eat_num; i++)
    {
        struct cs_EnvAction *meat = (struct cs_EnvAction *) malloc(
                sizeof(struct cs_EnvAction));
        meat->eat = eaif[i].eat;
        meat->count = eaif[i].count;    // copy count

        meat->next = mst->ealist;
        mst->ealist = meat;
    }

    /* create and copy actions information */
    for (i = 0; i < stif->act_num; i++)
    {
        struct cs_Action *mac = (struct cs_Action *) malloc(
                sizeof(struct cs_Action));
        mac->act = atif[i].act;
        mac->payoff = atif[i].payoff;

        mac->next = mst->atlist;
        mst->atlist = mac;
    }

    /* create and copy forward link information */
    for (i = 0; i < stif->lk_num; i++)
    {
        //mst->count++;       // mantain consistence between state count and ea cout

        Agent::State nst = flk[i].nst;    // Next state value
        struct cs_State *nmst = SearchState(nst);    // find if the Next state exists
        if (nmst != NULL)    // if so, inc count and make the link
        {
            // build the link
            dbgmoreprt("next state", "%ld exists, build the link\n", nst);
            LinkStates(mst, flk[i].eat, flk[i].act, nmst);
        }
        else    // for a non-existing Next state, we will create it, and build the link
        {
            // create a new previous state
            dbgmoreprt("next state", "%ld not exists, create it and build the link\n", nst);
            struct cs_State *nmst = NewState(nst);
            // Add to memory
            AddStateToMemory(nmst);
            // build the link
            LinkStates(mst, flk[i].eat, flk[i].act, nmst);
        }
    }

    /* UpdateState starting from mst's previous states. This update is IMPORTANT!
     * The information of mst need to be merged to memory by update, otherwise others parts of the memory will know
     * nothing about mst, then the information we got is useless.
     * */
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        UpdateState(bas->pstate);    // update previous state one by one

        nbas = bas->next;
    }

    return;
}

void CSMAgent::UpdateStateInfo(const State_Info_Header *stif)
{
    if (stif == NULL) return;

#ifdef _DEBUG_MORE_
    printf("---------------------- UpdateStateInfo: ---------------------");
    PrintStateInfo(stif);
#endif

    struct cs_State *mst = SearchState(stif->st);    // search for the state
    if (mst == NULL)    // state doesn't exists, use AddStateInfo() instead!
    {
        WARNNING(
                "UpdateStateInfo(): state %ld doesn't exist in memory, if you want to add it, using AddStateInfo()!\n",
                stif->st);
        return;
    }

    dbgmoreprt("UpdateStateInfo()", "update information of state %ld.\n", stif->st);

    unsigned char *p = (unsigned char *) stif;    // use point p to travel through each subpart
    // environment action information
    p += sizeof(struct State_Info_Header);
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // action information
    int len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct Action_Info *atif = (struct Action_Info *) p;

    // forward links information
    len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct Forward_Link_Info *flk = (struct Forward_Link_Info *) p;

    mst->count = stif->count;    // set count as the average sum
    mst->payoff = stif->payoff;
    /* It's very important to set original payoff here, some un-experiencing states my be created as recieved previous states,
     in this situation, the original payoff will remain empty, if we don't set it here, it will have no chance to be set.
     it doesn't matter if the original payoff set here is wrong, because when it experiences the state by itself, it'll get
     its own orignial payoff of this state from Avatar's OriginalPayoff() function. */
    mst->original_payoff = stif->original_payoff;

    // free old eactions, acitons, and links
    /* free atlist */
    struct cs_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        nac = ac->next;
        FreeAc(ac);
    }
    mst->atlist = NULL;    // set as NULL! It's very important!

    /* free ealist */
    struct cs_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        nea = ea->next;
        FreeEa(ea);
    }
    mst->ealist = NULL;    // set as NULL!

    /* free flist */
    struct cs_ForwardLink *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        nfas = fas->next;
        FreeFlk(fas);
    }
    mst->flist = NULL;    // set as NULL!

    // create and copy
    int i;
    /* create and copy ExActions information */
    for (i = 0; i < stif->eat_num; i++)
    {
        struct cs_EnvAction *meat = (struct cs_EnvAction *) malloc(
                sizeof(struct cs_EnvAction));
        meat->eat = eaif[i].eat;
        meat->count = eaif[i].count;    // copy count

        meat->next = mst->ealist;
        mst->ealist = meat;
    }

    /* create and copy actions information */
    for (i = 0; i < stif->act_num; i++)
    {
        struct cs_Action *mac = (struct cs_Action *) malloc(
                sizeof(struct cs_Action));
        mac->act = atif[i].act;
        mac->payoff = atif[i].payoff;

        mac->next = mst->atlist;
        mst->atlist = mac;
    }

    /* create and copy forward link information */
    for (i = 0; i < stif->lk_num; i++)
    {
        //mst->count++;       // mantain consistence between state count and ea cout

        Agent::State nst = flk[i].nst;    // Next state value
        struct cs_State *nmst = SearchState(nst);    // find if the Next state exists
        if (nmst != NULL)    // if so, inc count and make the link
        {
            // build the link
            dbgmoreprt("next state", "%ld exists, build the link\n", nst);
            LinkStates(mst, flk[i].eat, flk[i].act, nmst);
        }
        else    // for a non-existing Next state, we will create it, and build the link
        {
            // create a new previous state
            dbgmoreprt("next state", "%ld not exists, create it and build the link\n", nst);
            struct cs_State *nmst = NewState(nst);
            // Add to memory
            AddStateToMemory(nmst);
            // build the link
            LinkStates(mst, flk[i].eat, flk[i].act, nmst);
        }
    }

    /* UpdateState starting from mst's previous states. This update is IMPORTANT!
     * The information of mst need to be merged to memory by update, otherwise others parts of the memory will know
     * nothing about mst, then the information we got is useless.
     * */
    struct cs_BackwardLink *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        UpdateState(bas->pstate);    // update previous state one by one

        nbas = bas->next;
    }

    return;
}

void CSMAgent::DeleteState(State st)
{
    struct cs_State *mst = SearchState(st);
    return DeleteState(mst);
}

void CSMAgent::UpdateState(State st)
{
    struct cs_State *mst = SearchState(st);
    return UpdateState(mst);
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
