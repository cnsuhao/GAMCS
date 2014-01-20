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
#include "CSAgent.h"
#include "Debug.h"

/** \brief Load a specified state from a previous memory stored in database.
 * Note that it will load all states directly or indirectly connected to this state to the computer memory.
 * \param st state value
 * \return state struct of st in computer memory
 *
 */
void CSAgent::LoadState(Agent::State st)
{
    char si_buf[SI_MAX_SIZE];

    struct m_State *mst = SearchState(st);  // search memory for the state first
    if (mst == NULL)    // not found, create a new state struct
    {
        mst = NewState(st);
        /* Add to memory */
        mst->next = head;
        head = mst;
        states_map.insert(StatesMap::value_type(mst->st, mst)); // don't forget to update hash map
    }

    int len = DBFetchStateInfo(st, si_buf);     // get state information from database, return the size
    if (len == -1)       // should not happen, otherwise database corrupted!!
        ERROR("state: %ld should exist, but fetch from Database: %s returns NULL!\n", st, db_name.c_str());

    struct State_Info_Header *stif = (struct State_Info_Header *)si_buf;

    mst->mark = SAVED;      // it's SAVED when just load
    mst->st = stif->st;
    mst->original_payoff = stif->original_payoff;
    mst->payoff = stif->payoff;
    mst->count = stif->count;

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);

    unsigned char *p = (unsigned char *)stif;       // use point p to travel through even parts
    // point to actions
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    // point to environment actions
    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *)p;

    // point to backward links
    p += ea_len;
    struct BackLink *lk = (struct BackLink *)p;

    int i;
    /* build actions list */
    for (i=0; i<stif->act_num; i++)
    {
        struct m_Action *mac = NewAc(atif[i].act);
        mac->payoff = atif[i].payoff;

        mac->next = mst->atlist;
        mst->atlist = mac;
    }
    /* build exactions list */
    for (i=0; i<stif->eat_num; i++)
    {
        struct m_EnvAction *mea = NewEa(eaif[i].eat);
        mea->count = eaif[i].count;

        mea->next = mst->ealist;
        mst->ealist = mea;
    }

    /* build current state's backward list and previous state's forward lists */
    for (i=0; i<stif->lk_num; i++)
    {
        struct m_State *pmst = SearchState(lk[i].pst);  // search for previous state in memory
        if (pmst == NULL)   // not found, create one
        {
            pmst = NewState(lk[i].pst);
            /* Add to memory */
            pmst->next = head;
            head = pmst;
            states_map.insert(StatesMap::value_type(pmst->st, pmst));
        }
        /* build mst's backward list */
        struct m_BackArcState *mbas = NewBas();
        mbas->pstate = pmst;

        mbas->next = mst->blist;
        mst->blist = mbas;
        /* build pmst's forward list */
        struct m_ForwardArcState *mfas = NewFas(lk[i].peat, lk[i].pact);
        mfas->nstate = mst;

        mfas->next = pmst->flist;
        pmst->flist = mfas;
    }

    return;
}

/** \brief Initialize memory, if saved, loaded from database to computer memory, otherwise do nothing.
 *
 */

void CSAgent::InitMemory()
{
    if (db_name.empty())    // no database specified, do nothing
        return;

    int re = DBConnect();   // otherwise, load memory from database
    if (re == 0)        // successfully connected
    {
        char label[64] = "Loading: ";
        printf("Initializing Memory from Database: %s ", db_name.c_str());
        fflush(stdout);

        /* load memory information */
        struct m_Memory_Info *memif = DBFetchMemoryInfo();
        if (memif != NULL)
        {
            /*FIXME: last_st and last_act are not used yet */
            // update memory struct
            discount_rate = memif->discount_rate;
            threshold = memif->threshold;
            state_num = memif->state_num;
            lk_num = memif->lk_num;
            free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
        }

        /* load states information */
        Agent::State st;
        unsigned long index = 0;    // load states from database one by one
        while((st = DBStateByIndex(index)) != INVALID_STATE)
        {
            dbgmoreprt("DB: %s, LoadState: %ld\n", db_name.c_str(), st);
            LoadState(st);
            index++;
            PrintProcess(index, state_num, label);
        }
    }
    else    // connect database failed!
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
    }

    DBClose();
    return;
}

/** \brief Save current memroy to database, including states information and memory-level statistics.
 *
 */
void CSAgent::SaveMemory()
{
    if (db_name.empty())    // no database specified, no need to save
        return;

    char label[64] = "Saving: ";
    printf("Saving Memory to DataBase: %s ", db_name.c_str());
    int re = DBConnect();
    if (re == 0)
    {
        /* save memory information */
        DBAddMemoryInfo();
        /* save states information */
        char si_buf[SI_MAX_SIZE];
        struct m_State *mst, *nmst;
        unsigned long index = 0;
        for (mst=head; mst!=NULL; mst=nmst)
        {
            if (mst->mark == NEW)
            {
                dbgmoreprt("DB", "%s, state: %ld, Mark: %d\n", db_name.c_str(),mst->st, mst->mark);
                GetStateInfo(mst->st, si_buf);
                DBAddStateInfo((struct State_Info_Header *)si_buf);
            }
            else if (mst->mark == MODIFIED)
            {
                dbgmoreprt("DB", "%s, state: %ld, Mark: %d\n", db_name.c_str(),mst->st, mst->mark);
                GetStateInfo(mst->st, si_buf);
                DBUpdateStateInfo((struct State_Info_Header *)si_buf);
            }
            mst->mark = SAVED;

            index++;
            PrintProcess(index, state_num, label);
            nmst = mst->next;
        }
    }
    DBClose();
    return;
}

CSAgent::CSAgent() :
    state_num(0), lk_num(0), db_con(NULL), db_server(""), db_user(""), db_password(""), db_name(""),
    db_t_stateinfo("StateInfo"), db_t_meminfo("MemoryInfo"), head(NULL), cur_mst(NULL), actions_original_payoff(0.0)
{
    states_map.clear();
}

CSAgent::CSAgent(float dr, float th):
    Agent(dr, th), state_num(0), lk_num(0), db_con(NULL), db_server(""), db_user(""), db_password(""), db_name(""),
    db_t_stateinfo("StateInfo"), db_t_meminfo("MemoryInfo"), head(NULL), cur_mst(NULL), actions_original_payoff(0.0)
{
    states_map.clear();
}

CSAgent::~CSAgent()
{
    if (!db_name.empty())   // save memory if database not empty
        SaveMemory();
    FreeMemory();       // free computer memory
}

/** \brief search for state value in memory
 *
 * \param st state value
 * \return state struct, NULL for not exists
 *
 */

struct m_State *CSAgent::SearchState(Agent::State st) const
{
    StatesMap::const_iterator it = states_map.find(st); // find the state value in hash map
    if (it != states_map.end())     // found
        return (struct m_State *)(it->second);
    else
        return NULL;
}

/** \brief create a new state struct
* \param st state value to be created
* \return newly created state struct
*/
struct m_State *CSAgent::NewState(Agent::State st)
{
    struct m_State *mst = (struct m_State *)malloc(sizeof(struct m_State));
    // fill in default values
    mst->st = st;
    mst->original_payoff = 0.0;                 // any value, doesn't master
    mst->payoff = 0.0;         // set payoff to original payoff, it's important! //????? FIXME
    mst->count = 1;         // it's created when we first encounter it
    mst->flist = NULL;      // we just create a struct here, no links considered
    mst->blist = NULL;
    mst->mark = NEW;        // it's new, and should be saved
    mst->atlist = NULL;     // no actions or environment actions
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
    for (ac = mst->atlist; ac!=NULL; ac=nac)
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
    struct m_ForwardArcState *fas = (struct m_ForwardArcState *)malloc(sizeof(struct m_ForwardArcState));

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
    struct m_BackArcState *bas = (struct m_BackArcState *)malloc(sizeof(struct m_BackArcState));
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
struct m_Action* CSAgent::Act2Struct(Agent::Action act, const struct m_State *mst) const
{
    struct m_Action *ac, *nac;

    // walk through action list
    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        if (ac->act == act)
            return ac;

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
struct m_EnvAction* CSAgent::Eat2Struct(EnvAction eat, const struct m_State *mst) const
{
    struct m_EnvAction *ea, *nea;
    // walk through environment Agent::Action list
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        if (ea->eat == eat)
            return ea;

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
    struct m_EnvAction *ea = (struct m_EnvAction *)malloc(sizeof(struct m_EnvAction));

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
    struct m_Action *ac = (struct m_Action *)malloc(sizeof(struct m_Action));

    ac->act = act;
    ac->payoff = actions_original_payoff;
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
void CSAgent::LinkStates(struct m_State *pmst, EnvAction eat, Agent::Action act, struct m_State *mst)
{
    /* check if the link already exists, if so simply update the count of environment action */
    struct m_ForwardArcState *f, *nf;

    for (f = pmst->flist; f != NULL; f = nf)
    {
        if (f->nstate->st == mst->st &&
                f->act == act &&
                f->eat == eat)   // two links equal if and only if their states equal, and actions equal, and environment action equal
        {
            struct m_EnvAction *ea = Eat2Struct(eat, pmst);      // get the environment action struct
            ea->count++;        // inc count
            UpdateState(pmst);       // update previous state's payoff recursively
            return;     // done, return
        }

        nf = f->next;
    }

    /* link not exists, create a new link from pmst to mst */
    /* add mst to pmst's flist */
    struct m_ForwardArcState *fas = NewFas(eat, act);
    fas->nstate = mst;  // next state is mst
    // add fas to pmst's flist
    fas->next = pmst->flist;
    pmst->flist = fas;

    /* add pmst to mst's blist */
    struct m_BackArcState *bas = NewBas();
    bas->pstate = pmst;     // previous state is pmst
    // add bas to mst's blist
    bas->next = mst->blist;
    mst->blist = bas;

    lk_num++;       // update total link number
    /* A link is a combination of eaction AND aciton, either of them not existing means this link doesn't exist.
    *  So we have to figure out exactly which of them doesn't exist.
    */
    /* check eaction */
    struct m_EnvAction *ea = Eat2Struct(eat, pmst);

    if (ea == NULL)         // eaction not exist, add a new one to the ealist of pmst
    {
        struct m_EnvAction *nea = NewEa(eat);
        // add nea to pmst's environment action list
        nea->next = pmst->ealist;
        pmst->ealist = nea;
    }
    else                    // update count if exists
        ea->count++;


    /* check action */
    struct m_Action *ac = Act2Struct(act, pmst);
    if (ac == NULL)         // action not exist, add a new one to atlist of pmst and calculate it's payoff
    {
        struct m_Action *nac = NewAc(act);
        nac->payoff = 0.0; //CalActPayoff(nac->act, pmst);?
        // add nac to pmst's action list
        nac->next = pmst->atlist;
        pmst->atlist = nac;
    }
//    else                    // simply update payoff if exists
//        ac->payoff = CalActPayoff(ac->act, pmst);

    UpdateState(pmst);       // update payoff beginning from previous state recursively
    return;
}

/**
* \brief Find the maximum payoff of states linked in a specified environment action to one state
* \param eat specified environment action
* \param mst to which state the link is
* \return the maximum payoff
*/
float CSAgent::MaxPayoffInEat(EnvAction eat, const struct m_State *mst) const
{
    float max_pf = -FLT_MAX;    // set to a possibly minimun value
    struct m_State *nmst;
    struct m_ForwardArcState *fas, *nfas;

    // walk through the forward links
    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        if (fas->eat == eat)
        {
            nmst = fas->nstate;

            if (nmst->payoff > max_pf)  // record the bigger one
                max_pf = nmst->payoff;
        }

        nfas = fas->next;
    }

    return max_pf;
}

/**
* \brief Calculate the possibility of encountering an specified environment action
* \param ea environment
* \param mst the state struct
* \return the possibility
*/
float CSAgent::Prob(const struct m_EnvAction *ea, const struct m_State *mst) const
{
    float eacount = ea->count;
    float stcount = mst->count;
    return eacount / stcount;   // number of ea divided by the total number
}

/**
* \brief Calculate payoff of the specified state
* \param mst specified state
* \return payoff of the state
*/
float CSAgent::CalStatePayoff(const struct m_State *mst) const
{
    dbgmoreprt("CalStatePayoff(): state: %ld, count: %ld\n", mst->st, mst->count);

    float u0 = mst->original_payoff;
    float pf = u0;  // set initial value as original payoff
    float tmp;

    struct m_EnvAction *ea, *nea;

    // walk through all environment actions
    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        dbgmoreprt("", "=============== Ealist ===================\n");
        dbgmoreprt("eid: %ld, count: %ld\n", ea->eat, ea->count);
        tmp = Prob(ea, mst) * MaxPayoffInEat(ea->eat, mst);
        pf += tmp * discount_rate;  // accumulative total
        nea = ea->next;
    }
    dbgmoreprt("", "=============== Ealist End ===================\n");

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
    dbgmoreprt("UpdateState()", "state: %ld, payoff:%.1f\n", mst->st, payoff);

    if (fabsf(mst->payoff - payoff) >= threshold)    // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff  = payoff;
        dbgmoreprt("UpdateState()", "Change to payoff: %.1f\n", payoff);

        /* update backwards recursively */
        struct m_BackArcState *bas, *nbas;
        for (bas = mst->blist; bas != NULL; bas = nbas)
        {
            UpdateState(bas->pstate);
            nbas = bas->next;
        }
    }
    else
    {
        dbgmoreprt("UpdateState()", "Payoff no changes, it's smaller than %.1f\n", threshold);
    }

    /* update actions' payoff */
    struct m_Action *ac, *nac;
    for (ac=mst->atlist; ac!=NULL; ac=nac)
    {
        ac->payoff = CalActPayoff(ac->act, mst);
        nac = ac->next;
    }

    if (mst->mark == SAVED)
        mst->mark = MODIFIED;   // set mark to indicate the modification
    return;
}

/**
* \brief Return the state which is linked by a specified with specified environment action and action.
* \param eat environment action value
* \param act action value
* \param mst the state which links the needed state
* \return state struct needed, NULL if not found
*/
struct m_State *CSAgent::StateByEatAct(EnvAction eat, Agent::Action act, const struct m_State *mst) const
{
    struct m_ForwardArcState *fas, *nfas;
    for (fas=mst->flist; fas!=NULL; fas=nfas)
    {
        if (fas->eat == eat && fas->act == act)     // check both envir action and action value
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
    float ori_payoff = actions_original_payoff;             // original payoff of actions
    float payoff = ori_payoff;

    struct m_EnvAction *ea, *nea;
    struct m_State *nmst;

    // walk through the envir action list
    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        nmst = StateByEatAct(ea->eat, act, mst);    // find the states which are reached by performing this action
        if (nmst != NULL)
        {
            payoff += Prob(ea, mst) * (nmst->payoff);   // accumulative total
        }
        nea = ea->next;
    }
    dbgmoreprt("CalActPayoff()", "state: %ld, act: %ld, payoff:%.1f\n", mst->st, act, payoff);
    return payoff;
}

/**
* \brief Choose the best actions of a state from a candidate action list.
* \param mst the state
* \param candidate action list
* \return best actions
*/
std::vector<Agent::Action> CSAgent::BestActions(const struct m_State *mst, const std::vector<Agent::Action> &acts)
{
    float max_payoff = -FLT_MAX;
    float ori_payoff = actions_original_payoff;         // original payoff of actions
    float payoff;
    std::vector<Agent::Action> max_acts;

    max_acts.clear();
    // walk through every actions
    for (std::vector<Agent::Action>::const_iterator act = acts.begin();
            act!=acts.end(); ++act)
    {
        struct m_Action *mac = Act2Struct(*act, mst);   // get action struct from values

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
        else if (payoff == max_payoff)  // find an equal one, add it to the list
            max_acts.push_back(*act);
    }
    return max_acts;
}

/**
* \brief Update states in memory.
* \param oripayoff original payoff of current state
*/
void CSAgent::UpdateMemory(float oripayoff)
{
    if (pre_in == INVALID_STATE)    // previous state not exist, it's running for the first time
    {
        //NOTE: cur_mst is already set in MaxPayoffRule() function
        if (cur_mst == NULL)    // first time without memory, create current state struct in memory
        {
            cur_mst = NewState(cur_in);
            cur_mst->original_payoff = oripayoff; // set original payoff as given
            cur_mst->payoff = oripayoff; // set payoff as original payoff

            // add current state to memory
            cur_mst->next = head;
            head = cur_mst;

            states_map.insert(StatesMap::value_type(cur_mst->st, cur_mst)); // insert to hash map
            state_num++; // inc global state number
        }
        else    // state found in memory, simply update its count
        {
            cur_mst->count++;
            if (cur_mst->mark == SAVED)     // don't forget to change the storage flag
                cur_mst->mark = MODIFIED;
        }

        return;
    }

    /* previous state exists */
    struct m_State *pmst = SearchState(pre_in); // found previous state struct
    if (pmst == NULL)
        ERROR("Can not find previous state in memory, which should be existing!");

    //NOTE: cur_mst is already set in MaxPayoffRule() function
    if (cur_mst == NULL)    // currrent state struct not exists in memory, create it in memory, and link it to the previous state
    {
        cur_mst = NewState(cur_in);
        cur_mst->original_payoff = oripayoff;
        cur_mst->payoff = cur_mst->original_payoff;

        // add it to memory, and update counts
        cur_mst->next = head;
        head = cur_mst;
        states_map.insert(StatesMap::value_type(cur_mst->st, cur_mst));
        state_num++;

        EnvAction peat = cur_in - pre_in - pre_out;  // calcuate previous environment action
        LinkStates(pmst, peat, pre_out, cur_mst);   // build the link
    }
    else    // current state struct already exists, update the count and link it to the previous state (LinkStates will handle it if the link already exists.)
    {
        cur_mst->count++;

        if (cur_mst->mark == SAVED)
            cur_mst->mark = MODIFIED;

        EnvAction peat = cur_in - pre_in - pre_out;
        LinkStates(pmst, peat, pre_out, cur_mst);
    }

    return;
}

/**
* \brief Free computer memory used by the agent's memory.
*/
void CSAgent::FreeMemory()
{
    // free all states in turn
    struct m_State *mst, *nmst;
    for (mst=head; mst!=NULL; mst=nmst)
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
        ERROR("Cant remove state, state is NULL\n");

    // check if the state if connected by other states.
    // a state can only be removed when it's a ROOT state, which means no other states is linked to it.
    if (mst->blist != NULL)
    {
        dbgprt("RemoveState()", "RThis state is still linked to other states, can not be removed!\n");
        return;
    }

    // when a state is at root position, all the states which it's connected to (so called child states)
    // will be removed, and this process is done recursively.

    // walk through the forward list, remove all its child states
    struct m_ForwardArcState *fas, *nfas;
    for (fas=mst->flist; fas!=NULL; fas=nfas)
    {
        nfas = fas->next;
        RemoveState(fas->nstate);   // recursive call
        FreeFas(fas);
    }

    // remove ealist
    struct m_EnvAction *ea, *nea;
    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        nea = ea->next;
        FreeEa(ea);
    }

    // remove atlist
    struct m_Action *ac, *nac;
    for (ac=mst->atlist; ac!=NULL; ac=nac)
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
std::vector<Agent::Action> CSAgent::MaxPayoffRule(Agent::State st, const std::vector<Agent::Action> &acts)
{
    cur_mst = SearchState(st);  // get the state struct from state value
    std::vector<Agent::Action> re;

    if (cur_mst == NULL)        // first time to encounter this state, we know nothing about it, so no restriction applied, return the whole list
    {
        re = acts;
    }
    else                    // we have memories about this state, find the best action of it
    {
        re = BestActions(cur_mst, acts);
    }

    return re;
}

/**
* \brief Get information of specified state from memory, and fill it in the given buffer
* \param st the state whose information is requested
* \param buffer[out] buffer to store the state information
* \return the length of state information put in buffer, -1 for error
*/
int CSAgent::GetStateInfo(Agent::State st, void *buffer) const
{
    if (buffer == NULL) // error, buffer is null
    {
        dbgprt("GetStateInfo()", "incoming buffer is NULL, no space to put the information!\n");
        return -1;
    }

    struct m_State *mst;
    mst = SearchState(st);  // find the state struct in computer memory

    if (mst == NULL)    // error, not found
    {
        dbgprt("GetStateInfo()", "state: %ld not found!\n", st);
        return -1;
    }

    unsigned char *ptr = (unsigned char *)buffer;   // use point ptr to travel through subparts of state information

    // build a state information header
    struct State_Info_Header stif;
    stif.st = st;
    stif.original_payoff = mst->original_payoff;
    stif.payoff = mst->payoff;
    stif.count = mst->count;

    int act_num = 0;
    int eat_num = 0;
    int lk_num = 0;
    /* action information */
    ptr += sizeof(struct State_Info_Header);
    struct Action_Info acif;

    struct m_Action *ac, *nac;
    for (ac=mst->atlist; ac!=NULL; ac=nac)
    {
        acif.act = ac->act;
        acif.payoff = ac->payoff;
        memcpy(ptr, &acif, sizeof(struct Action_Info));
        ptr += sizeof(struct Action_Info);
        act_num++;
        if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)   // exceeds maximun buffer size, finish the filling
        {
            WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
            goto finish;
        }

        nac = ac->next;
    }

    /* EnvAction information */
    struct EnvAction_Info eaif;
    struct m_EnvAction *ea, *nea;
    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        eaif.count = ea->count;
        eaif.eat = ea->eat;
        memcpy(ptr, &eaif, sizeof(struct EnvAction_Info));
        ptr += sizeof(struct EnvAction_Info);
        eat_num++;
        if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)  // exceeds maximun buffer size, finish the filling
        {
            WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
            goto finish;
        }
        nea = ea->next;
    }

    /* links information */
    struct BackLink lk;
    struct m_BackArcState *bas, *nbas;
    for (bas=mst->blist; bas!=NULL; bas=nbas)
    {
        struct m_State *pmst = SearchState(bas->pstate->st);
        if (pmst == NULL)
            ERROR("GetStateInfo(): blist indicates a previous state existing, but search memory returns NULL!\n");
        struct m_ForwardArcState *fas, *nfas;
        for (fas=pmst->flist; fas!=NULL; fas=nfas)
        {
            if (fas->nstate->st == mst->st)
            {
                lk.pst = bas->pstate->st;
                lk.pact = fas->act;
                lk.peat = fas->eat;
                memcpy(ptr, &lk, sizeof(struct BackLink));
                ptr += sizeof(struct BackLink);
                lk_num++;
                if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)  // exceeds maximun buffer size, finish the filling
                {
                    WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
                    goto finish;
                }
            }
            nfas = fas->next;
        }
        nbas = bas->next;
    }

finish: // buffer is filled
    /* create state information with continous space */
    stif.act_num = act_num;
    stif.eat_num = eat_num;
    stif.lk_num = lk_num;

    memcpy(buffer, &stif, sizeof(struct State_Info_Header));    // fill in the header
    int length = ptr - (unsigned char *)buffer;
    return length;
}

/**
* \brief Merge a recieved state information to memory.
* \param stif the state information to be merged
* \return if the state information is finally accepted and merged, 1 for yes, 0 for no.
*/
int CSAgent::MergeStateInfo(const struct State_Info_Header *stif)
{
    unsigned char *p = (unsigned char *)stif;   // use point p to travel through each subpart
    // action information
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    // environment action information
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *)p;

    // backward links information
    len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct BackLink *lk = (struct BackLink *)p;

    int i;
    struct m_State *mst = SearchState(stif->st);    // search for the state
    int better = 0;         // if sender's info is better than mine

    if (mst == NULL)        // if it's new, accept it in memory
    {
        better = 1;         // anything is better than nothing

        mst = NewState(stif->st);
        /* Add to memory, and update counts */
        mst->next = head;
        head = mst;
        states_map.insert(StatesMap::value_type(mst->st, mst));
        state_num++;

        // copy state information
        mst->payoff = stif->payoff;
        mst->original_payoff = stif->original_payoff;
        mst->count = stif->count;

        /* copy actions information */
        for (i=0; i<stif->act_num; i++)
        {
            struct m_Action *mac = (struct m_Action *)malloc(sizeof(struct m_Action));
            mac->act = atif[i].act;
            mac->payoff = atif[i].payoff;

            mac->next = mst->atlist;
            mst->atlist = mac;
        }

        /* copy ExActions information */
        for (i=0; i<stif->eat_num; i++)
        {
            struct m_EnvAction *meat = (struct m_EnvAction *)malloc(sizeof(struct m_EnvAction));
            meat->eat = eaif[i].eat;
            meat->count = eaif[i].count;

            meat->next = mst->ealist;
            mst->ealist = meat;
        }

        /* copy links information */
        for (i=0; i<stif->lk_num; i++)
        {
            Agent::State pst = lk[i].pst;
            struct m_State *pmst = SearchState(pst);            // find if the previous state exists
            if (pmst != NULL)                                   // if so, make the link, otherwise do nothing
            {
                pmst->count++;                                  // as if we were coming from pmst, increase its count
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);
            }

        }
    } // mst == NULL
    else            // state already exists, merge the recieved one with it
    {
        if (stif->count <= mst->count)               // experience matters
        {
            better = 0;     // no lessons from a newbie
            return better;
        }

        // it's a veteran
        better = 1;
        /* stif->count > mst->count */
        mst->payoff = stif->payoff;

        /* merge action information */
        for (i=0; i<stif->act_num; i++)
        {
            struct m_Action *mac, *nmac;
            for (mac=mst->atlist; mac!=NULL; mac=nmac)
            {
                if ((mac->act == atif[i].act))
                {
                    mac->payoff = atif[i].payoff;
                    break;                 // one action should occur only once, break if we found one
                }
                nmac = mac->next;
            }

            if (mac == NULL)            // no corresponding action found in my own state, it's a new one, create it.
            {
                struct m_Action *nmac = (struct m_Action *)malloc(sizeof(struct m_Action));
                nmac->act = atif[i].act;
                nmac->payoff = atif[i].payoff;

                nmac->next = mst->atlist;
                mst->atlist = nmac;
            }
        }
        /* merge environment action information */
        long delta_count = 0;

        for (i = 0; i < stif->eat_num; i++)
        {
            struct m_EnvAction *meat, *nmeat;

            for (meat = mst->ealist; meat != NULL; meat = nmeat)
            {
                if (meat->eat == eaif[i].eat)
                {
                    delta_count += (eaif[i].count - meat->count);     // negative is ok
                    meat->count = eaif[i].count;        // use the recieved eat count
                    break;
                }

                nmeat = meat->next;
            }

            // new eat, create one
            if (meat == NULL)
            {
                struct m_EnvAction *neat = (struct m_EnvAction *)malloc(sizeof(struct m_EnvAction));
                neat->eat = eaif[i].eat;
                neat->count = eaif[i].count;

                neat->next = mst->ealist;
                mst->ealist = neat;

                delta_count += eaif[i].count;
            }
        }

        mst->count += delta_count;              // update state count

        /* links, make the link if previous state exists */
        for (i=0; i<stif->lk_num; i++)
        {
            Agent::State pst = lk[i].pst;
            struct m_State *pmst = SearchState(pst);
            if (pmst != NULL)
            {
                pmst->count++;
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);
            }
        }

    }
    if (better == 1 && mst->mark == SAVED)     // no modification to my state if I didn't accept it
        mst->mark = MODIFIED;
    return better;
}

/**
* \brief Pretty print State information
* \param specified State information header
*/
void CSAgent::PrintStateInfo(const struct State_Info_Header *stif)
{
    if (stif == NULL)
        return;

    int i = 0;
    printf("======================= State: %ld ===========================\n", stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n", stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions, Num: %d -----------------------\n", stif->act_num);
    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;
    for (i=0; i< stif->act_num; i++)
    {
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", atif[i].act, atif[i].payoff);
    }
    printf("------------------- ExActions, Num: %d ------------------------\n", stif->eat_num);
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *)p;
    for (i=0; i<stif->eat_num; i++)
    {
        printf("\t EnvAction: %ld,\t\t Count: %ld\n", eaif[i].eat, eaif[i].count);
    }
    printf("------------------------ BackLinks, Num: %d ---------------------------\n", stif->lk_num);
    len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct BackLink *lk = (struct BackLink *)p;
    for (i=0; i<stif->lk_num; i++)
    {
        printf("\t BackLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", lk[i].pst, lk[i].peat, lk[i].pact);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

/**
* \brief Set arguments for connecting database.
* \param srv database server location
* \param usr username of database
* \param passwd password of username
* \param name of the database
*/
void CSAgent::SetDBArgs(std::string srv, std::string usr, std::string passwd, std::string db)
{
    db_server = srv;
    db_user = usr;
    db_password = passwd;
    db_name = db;
    return;
}

/**
* \brief Connect to database.
* \return -1 for error, 0 for success
*/
int CSAgent::DBConnect()
{
    if (mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "could not initialize MySQL library!\n");
        return -1;
    }

    db_con = mysql_init(NULL);

    if (db_con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    if (mysql_real_connect(db_con, db_server.c_str(), db_user.c_str(), db_password.c_str(), NULL,
                           0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    /* create database if not exists */
    char db_string[128];
    sprintf(db_string, "CREATE DATABASE IF NOT EXISTS %s", db_name.c_str());
    if (mysql_query(db_con, db_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    mysql_select_db(db_con, db_name.c_str());               // use database
    /* create table if not exists */
    char tb_string[256];
    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, BackLinks BLOB) \
            ENGINE MyISAM ", db_name.c_str(), db_t_stateinfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(TimeStamp TIMESTAMP PRIMARY KEY, DiscountRate FLOAT, Threshold FLOAT, NumStates BIGINT, NumLinks BIGINT, LastState BIGINT, LastAction BIGINT) \
            ENGINE MyISAM ", db_name.c_str(), db_t_meminfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    return 0;
}

/**
* \brief Close database.
*/
void CSAgent::DBClose()
{
    mysql_close(db_con);
    return mysql_library_end();
}

/**
* \brief Get state value from database by specified index.
* \param index index
* \return state value of that index, INVALID_STATE for error or not found
*/
Agent::State CSAgent::DBStateByIndex(unsigned long index) const
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", db_t_stateinfo.c_str(), index);

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return INVALID_STATE;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBStateByIndex()", "result is  NULL!\n");
        return INVALID_STATE;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBStateByIndex()", "lengths is null\n");
        mysql_free_result(result);
        return INVALID_STATE;
    }
    Agent::State rs = atol(row[0]);

    mysql_free_result(result);          // free result
    return rs;
}

/**
* \brief Fetch state information from database, and put it in a buffer.
* \param st state value
* \param buffer buffer to put state information in
* \return length of the fetched state information, -1 if error
*/
int CSAgent::DBFetchStateInfo(Agent::State st, void *buffer) const
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);  // build mysql query

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBFetchStateInfo()", "result is NULL!\n");
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        dbgmoreprt("DBFetchStateInfo()", "Fields don't match!\n");
        return -1;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBFetchStateInfo()", "lengths is null\n");
        return -1;
    }

    unsigned char *ptr = (unsigned char *)buffer;       // use point ptr to travel through each subpart

    unsigned long ai_len = lengths[4];
    unsigned long ea_len = lengths[5];
    unsigned long lk_len = lengths[6];

    // build header
    struct State_Info_Header stif;
    stif.st = atol(row[0]);
    stif.original_payoff = atof(row[1]);
    stif.payoff = atof(row[2]);
    stif.count = atol(row[3]);

    stif.act_num = ai_len / sizeof(struct Action_Info);
    stif.eat_num = ea_len / sizeof(struct EnvAction_Info);
    stif.lk_num = lk_len / sizeof(struct BackLink);

    // copy header to buffer
    memcpy(ptr, &stif, sizeof(struct State_Info_Header));

    // action information part
    ptr += sizeof(struct State_Info_Header);
    memcpy(ptr, row[4], ai_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)      // buffer is full
    {
        WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    // environment action information part
    ptr += ai_len;
    memcpy(ptr, row[5], ea_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)          // CHECK NEEDED
    {
        WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    // backward links part
    ptr += ea_len;
    memcpy(ptr, row[6], lk_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)  // buffer is full
    {
        WARNNING("StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    mysql_free_result(result);          // free result

finish: // the buffer is filled up now
    int length = ptr - (unsigned char *)buffer;
    return length;
}

/**
* \brief Search for specified state in database.
* \param state value
* \return 1 if found, 0 if not
*/
int CSAgent::DBSearchState(Agent::State st) const
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    int re;
    if (result == NULL)
        re = 0;
    else
        re = 1;

    mysql_free_result(result);          // free result
    return re;
}

/**
* \brief Add state information to database.
* \param stif header pointed to state information
*/
void CSAgent::DBAddStateInfo(const struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str, "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, BackLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
            db_t_stateinfo.c_str(), stif->st, stif->original_payoff, stif->payoff, stif->count);    // first stag of building mysql insert query, actlist, eactlist and links are build below
    size_t str_len = strlen(str);

    // get lenght of several subparts
    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct BackLink);

    unsigned char *p = (unsigned char *)stif;   // use p to travel

    // point to action information part
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    // point to environment action information part
    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *)p;

    // point to backward link part
    p += ea_len;
    struct BackLink *lk = (struct BackLink *)p;

    char ai_chunk[2*ai_len +1]; // temporary buffer to put action information
    mysql_real_escape_string(db_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1]; // temporary buffer to put envir action info
    mysql_real_escape_string(db_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1]; // temporary buffer for links
    mysql_real_escape_string(db_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);     // final stag of building insert query

    if (mysql_real_query(db_con, query, len))       // perform the query, and insert st to database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;     // FIXME: indicator of error?
    }

    return;
}

/**
* \brief Update information of a state already exists in database.
* \param stif header pointed to the modified state information
*/
void CSAgent::DBUpdateStateInfo(const struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str, "UPDATE %s SET OriPayoff=%.2f, Payoff=%.2f, Count=%ld, ActInfos='%%s', ExActInfos='%%s', BackLinks='%%s' WHERE State=%ld",
            db_t_stateinfo.c_str(), stif->original_payoff, stif->payoff, stif->count, stif->st);    // first stage of building the update query
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct BackLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *)p;

    p += ea_len;
    struct BackLink *lk = (struct BackLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(db_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(db_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(db_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);     // final stage of building query

    if (mysql_real_query(db_con, query, len))   // perform the query, and update database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

/**
* \brief Delete a state from database by its value
* \param st state value to be delete
*/
void CSAgent::DBDeleteState(Agent::State st)
{
    char query_string[256];
    sprintf(query_string, "DELETE  FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);   // build delete query

    if (mysql_query(db_con, query_string))  // perform query
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

/**
* \brief Add memory statistics to database.
*/
void CSAgent::DBAddMemoryInfo()
{
    char query_str[256];

    sprintf(query_str, "INSERT INTO %s(TimeStamp, DiscountRate, Threshold, NumStates, NumLinks, LastState, LastAction) VALUES(NULL, %.2f, %.2f, %ld, %ld, %ld, %ld)",
            db_t_meminfo.c_str(), discount_rate, threshold, state_num, lk_num, pre_in, pre_out); // build insert query

    int len = strlen(query_str);
    if (mysql_real_query(db_con, query_str, len))   // perform query
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

/**
* \brief Fetch memory statistics from database.
* \return memory info struct, NULL if error
*/
struct m_Memory_Info *CSAgent::DBFetchMemoryInfo()
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s ORDER BY TimeStamp DESC LIMIT 1", db_t_meminfo.c_str());   // select the lastest one

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBFetchMemoryInfo()", "result is NULL!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBFetchMemoryInfo()", "lengths is null\n");
        mysql_free_result(result);
        return NULL;
    }

    struct m_Memory_Info *memif = (struct m_Memory_Info *)malloc(sizeof(struct m_Memory_Info));
    dbgprt("DB", "%s, Memory TimeStamp: %s\n", db_name.c_str(), row[0]);
    // fill in the memory struct
    memif->discount_rate = atof(row[1]);
    memif->threshold = atof(row[2]);
    memif->state_num = atol(row[3]);
    memif->lk_num = atol(row[4]);
    memif->last_st = atol(row[5]);
    memif->last_act = atol(row[6]);

    mysql_free_result(result);          // free result

    return memif;
}

/**
* \brief Pretty print process of load or save memory to database.
* \param current current progress
* \param total total amount
* \param label indicator label
*/
void CSAgent::PrintProcess(unsigned long current, unsigned long total, char *label) const
{
    double prcnt;
    int num_of_dots;
    char buffer[80] = {0};
    int width;
    /* get term width */
    FILE *fp;
    prcnt=1.0*current/total;
    fp = popen("stty size | cut -d\" \" -f2","r");
    fgets(buffer, sizeof(buffer),fp);
    pclose(fp);
    width = atoi(buffer);

    if ( width < 32)
    {
        printf("\e[1A%3d%% completed.\n", (int)(prcnt*100));
    }
    else
    {
        num_of_dots = width - 20;

        char *pline_to_print = (char *)malloc( sizeof(char)*width );
        int dots = (int)(num_of_dots*prcnt);

        memset(pline_to_print,0,width);
        memset(pline_to_print,'>',dots);
        memset(pline_to_print+dots, ' ', num_of_dots - dots);
        printf("\e[1A%s[%s] %3d%% \n", label, pline_to_print,(int)(prcnt*100));
        free(pline_to_print);
    }
    return;
}
