/**********************************************************************
 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "CSAgent.h"
#include "Mysql.h"
#include "Debug.h"

/** \brief Load a specified state from a previous memory stored in database.
 * Note that it will load all states directly or indirectly connected to this state to the computer memory.
 * \param st state value
 * \return state struct of st in computer memory
 *
 */
void CSAgent::LoadState(Agent::State st)
{
    struct m_State *mst = SearchState(st);    // search memory for the state first
    if (mst == NULL)    // not found, create a new empty state struct, this struct will be filled up below
    {
        mst = NewState(st);
        /* Add to memory */
        AddStateToMemory(mst);
    }

    struct State_Info_Header *stif = NULL;
    stif = storage->FetchStateInfo(st);    // get state information from database, return the size
    if (stif == NULL)    // should not happen, otherwise database corrupted!
        ERROR(
                "state: %ld should exist, but fetch from storage returns NULL, the database may be corrupted!\n",
                st);

    mst->mark = SAVED;    // it's SAVED when just loaded
    mst->st = stif->st;
    mst->original_payoff = stif->original_payoff;
    mst->payoff = stif->payoff;
    mst->count = stif->count;

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);

    unsigned char *p = (unsigned char *) stif;    // use point p to travel through even parts
    // point to actions
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;

    // point to environment actions
    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // point to backward links
    p += ea_len;
    struct BackLink *lk = (struct BackLink *) p;

    int i;
    /* build actions list */
    for (i = 0; i < stif->act_num; i++)
    {
        struct m_Action *mac = NewAc(atif[i].act);
        mac->payoff = atif[i].payoff;
        // add to mst's atlist
        mac->next = mst->atlist;
        mst->atlist = mac;
    }
    /* build exactions list */
    for (i = 0; i < stif->eat_num; i++)
    {
        struct m_EnvAction *mea = NewEa(eaif[i].eat);
        mea->count = eaif[i].count;
        // add to mst's ealist
        mea->next = mst->ealist;
        mst->ealist = mea;
    }

    /* build current state's backward list and previous state's forward lists */
    for (i = 0; i < stif->lk_num; i++)
    {
        struct m_State *pmst = SearchState(lk[i].pst);    // search for previous state in memory
        if (pmst == NULL)    // not found, create one
        {
            pmst = NewState(lk[i].pst);
            /* Add to memory */
            AddStateToMemory(pmst);
        }
        /* build mst's backward list */
        struct m_BackArcState *mbas = NewBas();
        mbas->pstate = pmst;
        // add to mst's blist
        mbas->next = mst->blist;
        mst->blist = mbas;
        /* build pmst's forward list */
        struct m_ForwardArcState *mfas = NewFas(lk[i].peat, lk[i].pact);
        mfas->nstate = mst;
        // add to mst's flist
        mfas->next = pmst->flist;
        pmst->flist = mfas;
    }

    free(stif);    // dont't forget to free

    return;
}

/** \brief Initialize memory, if saved, loaded from database to computer memory, otherwise do nothing.
 *
 */

void CSAgent::InitMemory()
{
    if (storage == NULL)    // no database specified, do nothing
        return;

    int re = storage->Connect();    // otherwise, load memory from database
    if (re == 0)    // successfully connected
    {
        char label[10] = "Loading: ";
        printf("Initializing Memory from Storage: ");
        fflush(stdout);

        /* load memory information */
        unsigned long saved_state_num = 0;
        struct Memory_Info *memif = storage->FetchMemoryInfo();
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
        Agent::State st;
        unsigned long index = 0;    // load states from database one by one
        while ((st = storage->StateByIndex(index)) != INVALID_STATE)
        {
            dbgmoreprt("InitMemory()", "LoadState: %ld\n", st);
            LoadState(st);
            index++;
            PrintProcess(index, saved_state_num, label);
        }
        // check if number of states consistent
        if (saved_state_num != state_num)
        {
            WARNNING(
                    "InitMemory(): Number of states not consistent,which by stored meminfo says to be %ld, but in stateinfo is %ld, the storage may be conrupted!\n",
                    saved_state_num, state_num);
        }
    }
    else    // connect database failed!
    {
        WARNNING("InitMemory(): Connecting storage failed!\n");
    }

    storage->Close();
    return;
}

/** \brief Save current memroy to database, including states information and memory-level statistics.
 *
 */
void CSAgent::SaveMemory()
{
    if (storage == NULL)    // no database specified, no need to save
        return;

    char label[10] = "Saving: ";
    printf("Saving Memory to: ");
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

        storage->AddMemoryInfo(memif);    // add to storage
        free(memif);    // free it

        /* save states information */
        struct State_Info_Header *stif = NULL;
        struct m_State *mst, *nmst;
        unsigned long index = 0;
        // walk through all state structs
        for (mst = head; mst != NULL; mst = nmst)
        {
            if (mst->mark == NEW)
            {
                dbgmoreprt("SaveMemory()", "state: %ld, Mark: %d\n", mst->st, mst->mark);
                stif = GetStateInfo(mst->st);
                assert(stif!=NULL);
                storage->AddStateInfo(stif);
                free(stif);    // free
            }
            else if (mst->mark == MODIFIED)
            {
                dbgmoreprt("SaveMemory()", "state: %ld, Mark: %d\n", mst->st, mst->mark);
                stif = GetStateInfo(mst->st);
                assert(stif!=NULL);
                storage->UpdateStateInfo(stif);
                free(stif);    // free
            }
            mst->mark = SAVED;    // update flag

            index++;
            PrintProcess(index, state_num, label);
            nmst = mst->next;
        }
    }
    storage->Close();
    return;
}

CSAgent::CSAgent() :
        state_num(0), lk_num(0), storage(NULL), head(NULL), cur_mst(NULL)
{
    states_map.clear();
}

CSAgent::CSAgent(float dr, float th) :
        Agent(dr, th), state_num(0), lk_num(0), storage(NULL), head(NULL), cur_mst(
        NULL)
{
    states_map.clear();
}

CSAgent::~CSAgent()
{
    if (storage != NULL)    // save memory if storage exists
        SaveMemory();
    FreeMemory();    // free computer memory
}

/** \brief search for state value in memory
 *
 * \param st state value
 * \return state struct, NULL for not exists
 *
 */

struct m_State *CSAgent::SearchState(Agent::State st) const
{
    StatesMap::const_iterator it = states_map.find(st);    // find the state value in hash map
    if (it != states_map.end())    // found
        return (struct m_State *) (it->second);
    else
        return NULL;
}

/** \brief create a new state struct
 * \param st state value to be created
 * \return newly created state struct
 */
struct m_State *CSAgent::NewState(Agent::State st)
{
    struct m_State *mst = (struct m_State *) malloc(sizeof(struct m_State));
    assert(mst!=NULL);
    // fill in default values
    mst->st = st;
    mst->original_payoff = 0.0;    // any value, doesn't master, it'll be set when used. Note: this value is also used for unseen previous state recieved in links from others.
    mst->payoff = 0.0;    // set payoff to original payoff, it's important! //??? FIXME
    mst->count = 1;    // it's created when we first encounter it
    mst->flist = NULL;    // we just create a struct here, no links considered
    mst->blist = NULL;
    mst->mark = NEW;    // it's new, and should be saved
    mst->atlist = NULL;    // no actions or environment actions
    mst->ealist = NULL;
    mst->next = NULL;
    return mst;
}

/** \brief Free a state struct and retrieve its computer memory
 * \param mst state struct
 *
 */
void CSAgent::FreeState(struct m_State *mst)
{
    // before free the struct itself, free its subparts first
    /* free atlist */
    struct m_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        nac = ac->next;
        FreeAc(ac);
    }

    /* free ealist */
    struct m_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        nea = ea->next;
        FreeEa(ea);
    }

    /* free flist */
    struct m_ForwardArcState *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        nfas = fas->next;
        FreeFas(fas);
    }

    /* free blist */
    struct m_BackArcState *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        nbas = bas->next;
        FreeBas(bas);
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

struct m_ForwardArcState *CSAgent::NewFas(EnvAction eat, Agent::Action act)
{
    struct m_ForwardArcState *fas = (struct m_ForwardArcState *) malloc(
            sizeof(struct m_ForwardArcState));

    fas->act = act;
    fas->eat = eat;
    fas->nstate = NULL;
    fas->next = NULL;
    return fas;
}

/** \brief Free a forward arc struct
 *
 * \param fas the struct to be freed
 *
 */
void CSAgent::FreeFas(struct m_ForwardArcState *fas)
{
    return free(fas);
}

/** \brief Create a new Back Arc struct
 *
 * \return a new back arc struct
 *
 */
struct m_BackArcState *CSAgent::NewBas()
{
    struct m_BackArcState *bas = (struct m_BackArcState *) malloc(
            sizeof(struct m_BackArcState));
    bas->pstate = NULL;
    bas->next = NULL;
    return bas;
}

/** \brief Free a back arc struct
 *
 */
void CSAgent::FreeBas(struct m_BackArcState *bas)
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
struct m_Action* CSAgent::Act2Struct(Agent::Action act,
        const struct m_State *mst) const
{
    struct m_Action *ac, *nac;

    // walk through action list
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        if (ac->act == act) return ac;

        nac = ac->next;
    }

    return NULL;
}

/** \brief Convert from an environment action value to a EnvAction struct
 *
 * \param eat environment action value
 * \param mst in which state struct to search
 * \return the environment action struct found, NULL if not found
 *
 */
struct m_EnvAction* CSAgent::Eat2Struct(EnvAction eat,
        const struct m_State *mst) const
{
    struct m_EnvAction *ea, *nea;
    // walk through environment Agent::Action list
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        if (ea->eat == eat) return ea;

        nea = ea->next;
    }

    return NULL;
}

/** \brief Create a new Environment action struct
 *
 * \param eat environment action value
 * \return a new environment action struct
 *
 */
struct m_EnvAction *CSAgent::NewEa(EnvAction eat)
{
    struct m_EnvAction *ea = (struct m_EnvAction *) malloc(
            sizeof(struct m_EnvAction));

    ea->eat = eat;
    ea->count = 1;
    ea->next = NULL;
    return ea;
}

void CSAgent::FreeEa(struct m_EnvAction *ea)
{
    return free(ea);
}

/** \brief Create a new action struct
 *
 * \param eat action value
 * \return a new action struct
 *
 */
struct m_Action *CSAgent::NewAc(Agent::Action act)
{
    struct m_Action *ac = (struct m_Action *) malloc(sizeof(struct m_Action));

    ac->act = act;
    ac->payoff = unseen_action_payoff;
    ac->next = NULL;
    return ac;
}

void CSAgent::FreeAc(struct m_Action *ac)
{
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
void CSAgent::LinkStates(struct m_State *pmst, EnvAction eat, Agent::Action act,
        struct m_State *mst)
{
    /* check if the link already exists, if so simply update the count of environment action */
    dbgmoreprt("Enter LinkStates()", "------------------------------------- Make Link: %ld == %ld + %ld => %ld\n", pmst->st, eat, act, mst->st);
    struct m_ForwardArcState *f, *nf;
    for (f = pmst->flist; f != NULL; f = nf)
    {
        if (f->nstate->st == mst->st && f->act == act && f->eat == eat)    // two links equal if and only if their states equal, and actions equal, and environment action equal
        {
            dbgmoreprt("", "link already exists, increase ea count only\n");
            struct m_EnvAction *ea = Eat2Struct(eat, pmst);    // get the environment action struct
            ea->count++;    // inc count
            return;    // done, return
        }

        nf = f->next;
    }

    dbgmoreprt("", "create new link...\n");
    /* link not exists, create a new link from pmst to mst */
    /* add mst to pmst's flist */
    struct m_ForwardArcState *fas = NewFas(eat, act);
    fas->nstate = mst;    // next state is mst
    // add fas to pmst's flist
    fas->next = pmst->flist;
    pmst->flist = fas;

    /* add pmst to mst's blist */
    struct m_BackArcState *bas = NewBas();
    bas->pstate = pmst;    // previous state is pmst
    // add bas to mst's blist
    bas->next = mst->blist;
    mst->blist = bas;

    lk_num++;    // update total link number
    /* A link is a combination of eaction AND aciton, either of them not existing means this link doesn't exist.
     *  So we have to figure out exactly which of them doesn't exist.
     */
    /* check eaction */
    struct m_EnvAction *ea = Eat2Struct(eat, pmst);

    if (ea == NULL)    // eaction not exist, add a new one to the ealist of pmst
    {
        struct m_EnvAction *nea = NewEa(eat);
        // add nea to pmst's environment action list
        nea->next = pmst->ealist;
        pmst->ealist = nea;
    }
    else
    {
        // update count if exists
        ea->count++;    // inc count
    }

    /* check action */
    struct m_Action *ac = Act2Struct(act, pmst);
    if (ac == NULL)    // action not exist, add a new one to atlist of pmst and calculate it's payoff
    {
        struct m_Action *nac = NewAc(act);
        // add nac to pmst's action list
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
float CSAgent::MaxPayoffInEat(EnvAction eat, const struct m_State *mst) const
{
    dbgmoreprt("Enter MaxPayoffEat()", "-------------- eat: %ld, state: %ld\n", eat, mst->st);
    float max_pf = -FLT_MAX;    // set to a possibly minimun value
    struct m_State *nmst;
    struct m_ForwardArcState *fas, *nfas;
    /* An env action may be on ealist but absent on flist, this could happen when the state information was recieved from others.
     * Recall the struction of state information, state information sent contains ealist about this state, but the forward links are not included.
     * */
    bool eat_found_in_flist = false;    // indicate if an env action in ealist was found on forward list also.

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

    if (eat_found_in_flist == false) max_pf = unseen_eaction_maxpayoff;    // if the env action was not found on flist, an default maxpayoff will be used.

    return max_pf;
}

/**
 * \brief Calculate the possibility of encountering an specified environment action
 * \param ea environment
 * \param mst the state struct
 * \return the possibility
 */
float CSAgent::Prob(const struct m_EnvAction *ea,
        const struct m_State *mst) const
{
    unsigned long eacount = ea->count;
    // calculate the sum of env action counts
    unsigned long sum_eacount = 0;
    struct m_EnvAction *pea, *pnea;
    dbgprt("Prob", "------- state: %ld, count %ld, ", mst->st, mst->count);
    for (pea = mst->ealist; pea != NULL; pea = pnea)
    {
        sum_eacount += pea->count;
//        dbgprt("Prob", "eat: %ld, count %ld, ", pea->eat, pea->count);

        pnea = pea->next;
    }

    dbgprt("Prob", "sum: %ld\n", sum_eacount);

    float re = (1.0 / sum_eacount) * eacount;    // number of env actions divided by the total number
    /* do some checks below */
    // check if re is in range (0, 1]
    if (re < 0 || re > 1)    // check failed
    {
        ERROR(
                "Prob(): probability is %.2f, which must in range [0, 1]. state: %ld, eact is %ld, count is %ld, total eacount is %ld.\n",
                re, mst->st, ea->eat, eacount, sum_eacount);
    }
//
//    /* In every states loop, the count of conjoint state will temporarily be 1 bigger than the sum of its env action counts.
//     * otherwise something wrong might happen */
//    // check if total env action counts deviate too much from state count,
//    unsigned long stcount = mst->count;
//    unsigned long deviate = 0;
//
//    if ((deviate = labs(sum_eacount - stcount)) != 0)
//    {
//        ERROR("state %ld count: %ld and total env action counts: %ld, %ld deviation, this value shouldn't be bigger than one!\n", mst->st, stcount, sum_eacount, deviate);
//    }
//    else if (deviate != 0)
//    {
//        WARNNING("state %ld count: %ld and total env action counts: %ld, %ld deviation!\n", mst->st, stcount, sum_eacount, deviate);
//    }

    return re;
}

/**
 * \brief Calculate payoff of the specified state
 * \param mst specified state
 * \return payoff of the state
 */
float CSAgent::CalStatePayoff(const struct m_State *mst) const
{
    dbgmoreprt("\nCalStatePayoff()", "-------------------------------- state: %ld, count: %ld\n", mst->st, mst->count);

    float u0 = mst->original_payoff;
    float pf = u0;    // set initial value as original payoff
    float tmp;

    struct m_EnvAction *ea, *nea;

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
 * \param mst a specified state where the update begins
 */
void CSAgent::UpdateState(struct m_State *mst)
{
    /* update state's payoff */
    float payoff = CalStatePayoff(mst);
    dbgmoreprt("\nUpdateState()", "------------------------------------------- state: %ld, intial payoff:%.1f to %.1f\n", mst->st, mst->payoff, payoff);

    if (fabsf(mst->payoff - payoff) >= threshold)    // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff = payoff;
        dbgmoreprt("UpdateState()", "Change to payoff: %.1f\n", payoff);

        /* update backwards recursively */
        struct m_BackArcState *bas, *nbas;
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

    /* update actions' payoff */
    struct m_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        ac->payoff = CalActPayoff(ac->act, mst);
        nac = ac->next;
    }

    if (mst->mark == SAVED) mst->mark = MODIFIED;    // set mark to indicate the modification, no need to change if it's NEW
    return;
}

/**
 * \brief Return the state which is linked by a specified with specified environment action and action.
 * \param eat environment action value
 * \param act action value
 * \param mst the state which links the needed state
 * \return state struct needed, NULL if not found
 */
struct m_State *CSAgent::StateByEatAct(EnvAction eat, Agent::Action act,
        const struct m_State *mst) const
{
    struct m_ForwardArcState *fas, *nfas;
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
float CSAgent::CalActPayoff(Agent::Action act, const struct m_State *mst) const
{
    float ori_payoff = unseen_action_payoff;    // original payoff of actions
    float payoff = ori_payoff;

    struct m_EnvAction *ea, *nea;
    struct m_State *nmst;

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
std::vector<Agent::Action> CSAgent::BestActions(const struct m_State *mst,
        const std::vector<Agent::Action> &acts)
{
    float max_payoff = -FLT_MAX;
    float ori_payoff = unseen_action_payoff;    // original payoff of actions
    float payoff;
    std::vector<Agent::Action> max_acts;

    max_acts.clear();
    // walk through every actions
    for (std::vector<Agent::Action>::const_iterator act = acts.begin();
            act != acts.end(); ++act)
    {
        struct m_Action *mac = Act2Struct(*act, mst);    // get action struct from values

        if (mac != NULL)
            payoff = mac->payoff;
        else
            payoff = ori_payoff;    // an unseen action has default payoff

        if (payoff > max_payoff)    // find a bigger one, refill the max payoff action list
        {
            max_acts.clear();
            max_acts.push_back(*act);
            max_payoff = payoff;
        }
        else if (payoff == max_payoff)    // find an equal one, add it to the list
            max_acts.push_back(*act);
    }
    return max_acts;
}

/**
 * \brief Update states in memory. Note: This function should be called AFTER MaxPayoffRule() in every step!
 * \param oripayoff original payoff of current state
 */
void CSAgent::UpdateMemory(float oripayoff)
{
    dbgmoreprt("\nEnter UpdateMemory()", "-------------------------------------------------\n");
    if (pre_in == INVALID_STATE)    // previous state not exist, it's running for the first time
    {
        dbgmoreprt("", "Previous state not exists, create current state in memory.\n");
        //NOTE: cur_mst is already set in MaxPayoffRule() function
        if (cur_mst == NULL)    // create current state in memory
        {
            cur_mst = NewState(cur_in);
            cur_mst->original_payoff = oripayoff;    // set original payoff as given
            cur_mst->payoff = oripayoff;    // set payoff as original payoff

            // add current state to memory
            AddStateToMemory(cur_mst);
        }
        else    // state found, this could happen if others send state information to me before the first time I'm running
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
    struct m_State *pmst = SearchState(pre_in);    // found previous state struct
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
        // add it to memory
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
void CSAgent::FreeMemory()
{
    // free all states in turn
    struct m_State *mst, *nmst;
    for (mst = head; mst != NULL; mst = nmst)
    {
        nmst = mst->next;
        FreeState(mst);
    }
    states_map.clear();
}

/**
 * \brief Remove and free a specified state from computer memory.
 * \param mst the state to be removed
 */
void CSAgent::RemoveState(struct m_State *mst)
{
    if (mst == NULL)
    {
        WARNNING("RemoveState() - Try to remove a NULL state struct\n");
        return;
    }

    // check if the state if connected by other states.
    // a state can only be removed when it's a ROOT state, which means no other states is linked to it.
    if (mst->blist != NULL)
    {
        dbgprt("RemoveState()",
                "RThis state is still linked to other states, can not be removed!\n");
        return;
    }

    // when a state is at root position, all the states which it's connected to (so called child states)
    // will be removed, and this process is done recursively.

    // walk through the forward list, remove all its child states
    struct m_ForwardArcState *fas, *nfas;
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        nfas = fas->next;
        RemoveState(fas->nstate);    // recursive call
        FreeFas(fas);
    }

    // remove ealist
    struct m_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        nea = ea->next;
        FreeEa(ea);
    }

    // remove atlist
    struct m_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        nac = ac->next;
        FreeAc(ac);
    }

    // remove the state itself
    return FreeState(mst);
}

/**
 * \brief Implementation of the Maximun Payoff Rule (MPR).
 * \param st state which is concerned
 * \param acts the candidate action list
 * \return actions choosen by MPR
 */
std::vector<Agent::Action> CSAgent::MaxPayoffRule(Agent::State st,
        const std::vector<Agent::Action> &acts)
{
    dbgmoreprt("Enter MaxPayoffRule() ", "-------------------------------------------------- State: %ld\n", st);
    cur_mst = SearchState(st);    // get the state struct from state value
    std::vector<Agent::Action> re;

    if (cur_mst == NULL)    // first time to encounter this state, we know nothing about it, so no restriction applied, return the whole list
    {
        dbgprt("", "State not found in memory.\n");
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
struct State_Info_Header *CSAgent::GetStateInfo(Agent::State st) const
{
    if (st == INVALID_STATE)    // check if valid
    {
        dbgprt("GetStateInfo()", "invalid state value\n");
        return NULL;
    }

    struct m_State *mst;
    mst = SearchState(st);    // find the state struct in computer memory

    if (mst == NULL)    // error, not found
    {
        dbgprt("GetStateInfo()", "state: %ld not found in memory!\n", st);
        return NULL;
    }

    /* numbers */
    int act_num = 0;    // number of actions
    int eat_num = 0;    // number of envir actions
    int lk_num = 0;    // number of backward links
    /* get numbers */
    struct m_Action *ac, *nac;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        act_num++;
        nac = ac->next;
    }

    struct m_EnvAction *ea, *nea;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        eat_num++;
        nea = ea->next;
    }

    struct m_BackArcState *bas, *nbas;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        lk_num++;
        nbas = bas->next;
    }

    // allocate memory
    int stif_size = sizeof(struct State_Info_Header)
            + act_num * sizeof(struct Action_Info)
            + eat_num * sizeof(struct EnvAction_Info)
            + lk_num * sizeof(struct BackLink);

    struct State_Info_Header *stif = (struct State_Info_Header *) malloc(
            stif_size);

    // fill the header
    stif->st = st;
    stif->original_payoff = mst->original_payoff;
    stif->payoff = mst->payoff;
    stif->count = mst->count;
    stif->act_num = act_num;
    stif->eat_num = eat_num;
    stif->lk_num = lk_num;
    stif->size = stif_size;

    unsigned char *ptr = (unsigned char *) stif;    // use point ptr to travel through subparts of state information
    /* fill action information */
    ptr += sizeof(struct State_Info_Header);
    struct Action_Info acif;
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        acif.act = ac->act;
        acif.payoff = ac->payoff;
        memcpy(ptr, &acif, sizeof(struct Action_Info));
        ptr += sizeof(struct Action_Info);

        nac = ac->next;
    }

    /* EnvAction information */
    struct EnvAction_Info eaif;
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        eaif.count = ea->count;
        eaif.eat = ea->eat;
        memcpy(ptr, &eaif, sizeof(struct EnvAction_Info));
        ptr += sizeof(struct EnvAction_Info);

        nea = ea->next;
    }

    /* links information */
    struct BackLink lk;
    for (bas = mst->blist; bas != NULL; bas = nbas)
    {
        struct m_State *pmst = SearchState(bas->pstate->st);
        if (pmst == NULL)
            ERROR(
                    "GetStateInfo(): blist indicates a previous state existing, but search memory returns NULL!\n");
        struct m_ForwardArcState *fas, *nfas;
        for (fas = pmst->flist; fas != NULL; fas = nfas)
        {
            if (fas->nstate->st == mst->st)
            {
                lk.pst = bas->pstate->st;
                lk.pact = fas->act;
                lk.peat = fas->eat;
                memcpy(ptr, &lk, sizeof(struct BackLink));
                ptr += sizeof(struct BackLink);
            }
            nfas = fas->next;
        }
        nbas = bas->next;
    }

    if ((ptr - (unsigned char *) stif) != stif_size)    // check size
    {
        ERROR("GetStateInfo(): state information header size not match!\n");
    }

    return stif;
}

/**
 * \brief Merge a recieved state information to memory.
 * \param stif the state information to be merged
 * \return if the state information is finally accepted and merged, 1 for yes, 0 for no.
 */
void CSAgent::MergeStateInfo(const struct State_Info_Header *stif)
{
    dbgmoreprt("\nEnter MergeStateInfo()", "--------------------------------- state information to be merged: \n");
#ifdef _DEBUG_MORE_
    PrintStateInfo(stif);
#endif
    unsigned char *p = (unsigned char *) stif;    // use point p to travel through each subpart
    // action information
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;

    // environment action information
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // backward links information
    len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct BackLink *lk = (struct BackLink *) p;

    int i;
    struct m_State *mst = SearchState(stif->st);    // search for the state

    if (mst == NULL)    // if it's new, accept it in memory
    {
        dbgmoreprt("",
                " it's a new state: %ld, create it in memory.\n", stif->st);

        mst = NewState(stif->st);
        // copy state information
        mst->count = stif->count;    // copy count
        mst->payoff = stif->payoff;
        mst->original_payoff = stif->original_payoff;    // the original payoff is what really is important

        /* Add to memory */
        AddStateToMemory(mst);

        /* create and copy actions information */
        for (i = 0; i < stif->act_num; i++)
        {
            struct m_Action *mac = (struct m_Action *) malloc(
                    sizeof(struct m_Action));
            mac->act = atif[i].act;
            mac->payoff = atif[i].payoff;

            mac->next = mst->atlist;
            mst->atlist = mac;
        }

        /* create and copy ExActions information */
        for (i = 0; i < stif->eat_num; i++)
        {
            struct m_EnvAction *meat = (struct m_EnvAction *) malloc(
                    sizeof(struct m_EnvAction));
            meat->eat = eaif[i].eat;
            meat->count = eaif[i].count;    // copy count

            meat->next = mst->ealist;
            mst->ealist = meat;
        }

        /* create and copy backward links information */
        for (i = 0; i < stif->lk_num; i++)
        {
            Agent::State pst = lk[i].pst;    // previous state value
            struct m_State *pmst = SearchState(pst);    // find if the previous state exists
            if (pmst != NULL)    // if so, make the link, otherwise update count, the link checking and env action count updating are handled by LinkStates()
            {
                // build the link
                dbgmoreprt("previous state", "%ld exists, build the link\n", pst);
                pmst->count++;    // state count means the count we experience it either by reaching it or hearing of it.
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);    // LinkStates() will take care of the situation where the link is already existing, and it will also call UpdateState() to refresh memroy
            }
            else    // for a non-existing previous state, we will create it, and build the link
            {
                // create a new previous state
                dbgmoreprt("previous state", "%ld not exists, create it and build the link\n", pst);
                struct m_State *npmst = NewState(pst);
                npmst->original_payoff = unseen_state_payoff;    // I don't know anything except the state value of this state, set its original payoff to default unseen_state_payoff
                // add to memory
                AddStateToMemory(npmst);
                // build the link
                LinkStates(npmst, lk[i].peat, lk[i].pact, mst);    // npmst will be updated in LinkStates() funciton
            }
        }
    }    // mst == NULL
    else    // state already exists, merge the recieved one with it
    {
        dbgmoreprt("MergeStateInfo()", "state exists, do the merge.\n");

        mst->count = round((mst->count + stif->count) / 2);    // !!!set count as the average sum

        dbgmoreprt("state count round to ", "%ld\n", mst->count);
        mst->payoff = stif->payoff;    // meanless to set payoff, since it's calculated on fly
        /* It's very important to set original payoff here, some un-experiencing states my be created as recieved previous states,
         in this situation, the original payoff will remain empty, if we don't set it here, it will have no chance to be set.
         it doesn't matter if the original payoff set here is wrong, because when it experiences the state by itself, it'll get
         its own orignial payoff of this state from Avatar's OriginalPayoff() function. */
        mst->original_payoff = stif->original_payoff;

        /* merge action information, if an action doesn't exist, create it and copy payoff, otherwise copy payoff */
        for (i = 0; i < stif->act_num; i++)    // for each action in recieving list
        {
            struct m_Action *mac, *nmac;
            // walk through my atlist, find the corresponding action in my atlist
            for (mac = mst->atlist; mac != NULL; mac = nmac)
            {
                if ((mac->act == atif[i].act))    // found
                {
                    mac->payoff = atif[i].payoff;    // copy payoff
                    break;    // one action should occur only once, break if we found one
                }
                nmac = mac->next;
            }

            if (mac == NULL)    // no corresponding action found in my own state, it's a new one, create it.
            {
                struct m_Action *nmac = (struct m_Action *) malloc(
                        sizeof(struct m_Action));
                nmac->act = atif[i].act;
                nmac->payoff = atif[i].payoff;    // copy payoff
                // add to atlist
                nmac->next = mst->atlist;
                mst->atlist = nmac;
            }
        }

        /* merge environment action information, if an env action doesn't exist, create it and copy count, otherwise add up the count */
        // for all env action, divide its count by 2 first
        struct m_EnvAction *meat, *nmeat;
        for (meat = mst->ealist; meat != NULL; meat = nmeat)
        {
            meat->count = round(meat->count / 2);

            nmeat = meat->next;
        }

        // add recieved count as the other half
        for (i = 0; i < stif->eat_num; i++)    // for each env action in recieving list
        {

            // walk through my ealist to find the corresponding one
            for (meat = mst->ealist; meat != NULL; meat = nmeat)
            {
                if (meat->eat == eaif[i].eat)    // found
                {
                    meat->count += round(eaif[i].count / 2);    // set the env action count as average sum

                    break;
                }

                nmeat = meat->next;
            }

            // not found, create a new one
            if (meat == NULL)
            {
                struct m_EnvAction *neat = (struct m_EnvAction *) malloc(
                        sizeof(struct m_EnvAction));
                neat->eat = eaif[i].eat;
                neat->count = round(eaif[i].count / 2);    // average as well

                // add to ealist
                neat->next = mst->ealist;
                mst->ealist = neat;
            }
        }

        /* create and copy backward links information */
        for (i = 0; i < stif->lk_num; i++)
        {
            Agent::State pst = lk[i].pst;
            struct m_State *pmst = SearchState(pst);    // find if the previous state exists
            if (pmst != NULL)    // if so, make the link, otherwise update count, the link checking and env action count updating are handled by LinkStates()
            {
                dbgmoreprt("previous state", "%ld exists, build the link\n", pst);
                // build the link
                pmst->count++;    // state count means the count we experience it either by reaching it or hearing of it.
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);    // LinkStates() will take care of the situation where the link is already existing, and it will also call UpdateState() to refresh memroy
            }
            else    // for a non-existing previous state, we will create it, and build the link
            {
                dbgmoreprt("previous state", "%ld not exists, create it and build the link\n", pst);
                // create a new previous state
                struct m_State *npmst = NewState(pst);
                npmst->original_payoff = unseen_state_payoff;    // I don't know anything except the state value of this state, set its original payoff to default unseen_state_payoff
                // add to memory
                AddStateToMemory(npmst);
                // build the link
                LinkStates(npmst, lk[i].peat, lk[i].pact, mst);    // npmst will be updated in LinkStates() funciton
            }
        }

    }
    UpdateState(mst);    // update state

    return;
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void CSAgent::PrintStateInfo(const struct State_Info_Header *stif)
{
    if (stif == NULL) return;

    int i = 0;
    printf("======================= State: %ld ===========================\n",
            stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n",
            stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions, Num: %d -----------------------\n",
            stif->act_num);
    unsigned char *p = (unsigned char *) stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;
    for (i = 0; i < stif->act_num; i++)
    {
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", atif[i].act,
                atif[i].payoff);
    }
    printf("------------------- ExActions, Num: %d ------------------------\n",
            stif->eat_num);
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;
    for (i = 0; i < stif->eat_num; i++)
    {
        printf("\t EnvAction: %ld,\t\t Count: %ld\n", eaif[i].eat,
                eaif[i].count);
    }
    printf(
            "------------------------ BackLinks, Num: %d ---------------------------\n",
            stif->lk_num);
    len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct BackLink *lk = (struct BackLink *) p;
    for (i = 0; i < stif->lk_num; i++)
    {
        printf("\t BackLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", lk[i].pst,
                lk[i].peat, lk[i].pact);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

/**
 * \brief Pretty print process of load or save memory to database.
 * \param current current progress
 * \param total total amount
 * \param label indicator label
 */
void CSAgent::PrintProcess(unsigned long current, unsigned long total,
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

void CSAgent::AddStateToMemory(struct m_State *nstate)
{
    nstate->next = head;
    head = nstate;
    state_num++;
    states_map.insert(StatesMap::value_type(nstate->st, nstate));    // don't forget to update hash map
}
