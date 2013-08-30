/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "SimAgent.h"

struct m_State *SimAgent::LoadState(State st)
{
    struct m_State *mst = SearchState(st);
    if (mst == NULL)
    {
        char si_buf[SI_MAX_SIZE];
        int len = DBFetchStateInfo(st, si_buf);
        if (len == -1)       // should not happen, otherwise database corrupted!!
            ERROR("State: %ld should exist, but fetch from Database: %s returns NULL!\n", st, db_name.c_str());

        struct State_Info_Header *stif = (struct State_Info_Header *)si_buf;
        mst = NewState(st);
        mst->mark = SAVED;      // it's SAVED when just load
        mst->st = stif->st;
        mst->original_payoff = stif->original_payoff;
        mst->payoff = stif->payoff;
        mst->count = stif->count;
        /* Add to memory */
        mst->next = head;
        head = mst;
        states_map.insert(StatesMap::value_type(mst->st, mst));

        unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
        unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);

        unsigned char *p = (unsigned char *)stif;
        p += sizeof(struct State_Info_Header);
        struct Action_Info *atif = (struct Action_Info *)p;

        p += ai_len;
        struct ExAction_Info *eaif = (struct ExAction_Info *)p;

        p += ea_len;
        struct pLink *lk = (struct pLink *)p;

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
            struct m_ExAction *mea = NewEa(eaif[i].eat);
            mea->count = eaif[i].count;

            mea->next = mst->ealist;
            mst->ealist = mea;
        }

        /* build current state's backward list and previous state's forward lists */
        for (i=0; i<stif->lk_num; i++)
        {
            struct m_State *pmst = LoadState(lk[i].pst);
            /* build backward list */
            struct m_BackArcState *mbas = NewBas();
            mbas->pstate = pmst;

            mbas->next = mst->blist;
            mst->blist = mbas;

            struct m_ForwardArcState *mfas = NewFas(lk[i].peat, lk[i].pact);
            mfas->nstate = mst;

            mfas->next = pmst->flist;
            pmst->flist = mfas;
        }
    }
    return mst;
}

void SimAgent::InitMemory()
{
    if (db_name.empty())
        return;

    int re = DBConnect();
    if (re == 0)
    {
        printf("Initializing Memory from Database: %s .", db_name.c_str());
        fflush(stdout);
        /* load memory information */
        struct m_Memory_Info *memif = DBFetchMemoryInfo();
        if (memif != NULL)
        {
            /* last_st and last_act are not used yet */
            discount_rate = memif->discount_rate;
            threshold = memif->threshold;
            state_num = memif->state_num;
            lk_num = memif->lk_num;
            free(memif);
        }
        printf("..");
        fflush(stdout);
        /* load states information */
        State st;
        unsigned long index = 0;
        while((st = DBStateByIndex(index)) != -1)
        {
            dbgmoreprt("DB: %s, LoadState: %ld\n", db_name.c_str(), st);
            LoadState(st);
            index++;
            printf(".");
            fflush(stdout);
        }
    }
    else
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
    }
    printf("\n");
    DBClose();
    return;
}

void SimAgent::SaveMemory()
{
    if (db_name.empty())
        return;

    printf("Saving Memory to DataBase: %s .", db_name.c_str());
    fflush(stdout);
    int re = DBConnect();
    if (re == 0)
    {
        /* save memory information */
        DBAddMemoryInfo();
        printf("..");
        fflush(stdout);
        /* save states information */
        char si_buf[SI_MAX_SIZE];
        struct m_State *mst, *nmst;
        for (mst=head; mst!=NULL; mst=nmst)
        {
            if (mst->mark == NEW)
            {
                dbgmoreprt("DB: %s, State: %ld, Mark: %d\n", db_name.c_str(),mst->st, mst->mark);
                GetStateInfo(mst->st, si_buf);
                DBAddStateInfo((struct State_Info_Header *)si_buf);
            }
            else if (mst->mark == MODIFIED)
            {
                dbgmoreprt("DB: %s, State: %ld, Mark: %d\n", db_name.c_str(),mst->st, mst->mark);
                GetStateInfo(mst->st, si_buf);
                DBUpdateStateInfo((struct State_Info_Header *)si_buf);
            }
            printf(".");
            fflush(stdout);
            mst->mark = SAVED;

            nmst = mst->next;
        }
    }
    printf("\n");
    DBClose();
    return;
}

SimAgent::SimAgent():Agent()
{
    //ctor
    state_num = lk_num = 0;
    head = NULL;
    states_map.clear();

    db_con = NULL;
    db_server = "";
    db_user = "";
    db_password = "";
    db_name = "";
    db_t_stateinfo = "StateInfo";
    db_t_meminfo = "MemoryInfo";

    cur_mst = NULL;
    cur_st = -1;
}

SimAgent::SimAgent(float dr, float th):Agent(dr, th)
{
    state_num = lk_num = 0;
    head = NULL;
    states_map.clear();

    db_con = NULL;
    db_server = "";
    db_user = "";
    db_password = "";
    db_name = "";
    db_t_stateinfo = "StateInfo";
    db_t_meminfo = "MemoryInfo";

    cur_mst = NULL;
    cur_st = -1;
}

SimAgent::~SimAgent()
{
    //dtor
    dbgmoreprt("Enter ~SimAgent()\n");
    if (!db_name.empty())
        SaveMemory();
    FreeMemory();
}

struct m_State *SimAgent::SearchState(State st) const
{
    StatesMap::const_iterator it = states_map.find(st);
    if (it != states_map.end())     // found
        return (struct m_State *)(it->second);
    else
        return NULL;
}

struct m_State *SimAgent::NewState(State st)
{
    struct m_State *mst = (struct m_State *)malloc(sizeof(struct m_State));
    mst->st = st;
    mst->original_payoff = 1.0;                 // 1.0 as default
    mst->payoff = mst->original_payoff;         // set payoff to original payoff, it's important!
    mst->count = 1;
    mst->flist = NULL;
    mst->blist = NULL;
    mst->mark = NEW;
    mst->atlist = NULL;
    mst->ealist = NULL;
    mst->next = NULL;
    return mst;
}

void SimAgent::FreeState(struct m_State *mst)
{
    /* free atlist */
    struct m_Action *ac, *nac;
    for (ac = mst->atlist; ac!=NULL; ac=nac)
    {
        nac = ac->next;
        FreeAc(ac);
    }

    /* free ealist */
    struct m_ExAction *ea, *nea;
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

struct m_ForwardArcState *SimAgent::NewFas(ExAction eat, Action act)
{
    struct m_ForwardArcState *fas = (struct m_ForwardArcState *)malloc(sizeof(struct m_ForwardArcState));

    fas->act = act;
    fas->eat = eat;
    fas->nstate = NULL;
    fas->next = NULL;
    return fas;
}

void SimAgent::FreeFas(struct m_ForwardArcState *fas)
{
    return free(fas);
}

struct m_BackArcState *SimAgent::NewBas()
{
    struct m_BackArcState *bas = (struct m_BackArcState *)malloc(sizeof(struct m_BackArcState));
    bas->pstate = NULL;
    bas->next = NULL;
    return bas;
}

void SimAgent::FreeBas(struct m_BackArcState *bas)
{
    return free(bas);
}

struct m_Action* SimAgent::Act2Struct(Action act, struct m_State *mst)
{
    struct m_Action *ac, *nac;

    for (ac = mst->atlist; ac != NULL; ac = nac)
    {
        if (ac->act == act)
            return ac;

        nac = ac->next;
    }

    return NULL;
}

struct m_ExAction* SimAgent::Eat2Struct(ExAction eat, struct m_State *mst)
{
    struct m_ExAction *ea, *nea;

    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        if (ea->eat == eat)
            return ea;

        nea = ea->next;
    }

    return NULL;
}

struct m_ExAction *SimAgent::NewEa(ExAction eat)
{
    struct m_ExAction *ea = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));

    ea->eat = eat;
    ea->count = 1;
    ea->next = NULL;
    return ea;
}

void SimAgent::FreeEa(struct m_ExAction *ea)
{
    return free(ea);
}

struct m_Action *SimAgent::NewAc(Action act)
{
    struct m_Action *ac = (struct m_Action *)malloc(sizeof(struct m_Action));

    ac->act = act;
    ac->payoff = 0;
    ac->next = NULL;
    return ac;
}

void SimAgent::FreeAc(struct m_Action *ac)
{
    return free(ac);
}

void SimAgent::LinkStates(struct m_State *pmst, ExAction eat, Action act, struct m_State *mst)
{
    /* check if the link already exists, if so simply update the count of eaction */
    struct m_ForwardArcState *f, *nf;

    for (f = pmst->flist; f != NULL; f = nf)
    {
        if (f->nstate->st == mst->st &&
                f->act == act &&
                f->eat == eat)              //
        {
            struct m_ExAction *ea = Eat2Struct(eat, pmst);
            ea->count++;
            UpdateState(pmst);       // update previous state's payoff recursively
            return;
        }

        nf = f->next;
    }

    /* link not exists, create a new link from pmst to mst */
    /* add mst to pmst's flist */
    struct m_ForwardArcState *fas = NewFas(eat, act);
    fas->nstate = mst;
    fas->next = pmst->flist;
    pmst->flist = fas;

    /* add pmst to mst's blist */
    struct m_BackArcState *bas = NewBas();
    bas->pstate = pmst;
    bas->next = mst->blist;
    mst->blist = bas;

    lk_num++;       // update total arc number
    /* A link is a combination of eaction AND aciton, either of them not exist means link doesn't exist.
    *  So we have to identify exactly which of them doesn't exist.
    */
    /* check eaction */
    struct m_ExAction *ea = Eat2Struct(eat, pmst);

    if (ea == NULL)         // eaction not exist, add a new one to the ealist of pmst
    {
        struct m_ExAction *nea = NewEa(eat);
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
        nac->payoff = 0.0; //CalActPayoff(nac->act, pmst);
        nac->next = pmst->atlist;
        pmst->atlist = nac;
    }
//    else                    // simply update payoff if exists
//        ac->payoff = CalActPayoff(ac->act, pmst);

    UpdateState(pmst);       // update previous state's payoff recursively
    return;
}

float SimAgent::MaxPayoffInEat(ExAction eat, struct m_State *mst)
{
    float max_pf = -FLT_MAX;
    struct m_State *nmst;
    struct m_ForwardArcState *fas, *nfas;

    for (fas = mst->flist; fas != NULL; fas = nfas)
    {
        if (fas->eat == eat)
        {
            nmst = fas->nstate;

            if (nmst->payoff > max_pf)
                max_pf = nmst->payoff;
        }

        nfas = fas->next;
    }

    return max_pf;
}

float SimAgent::Prob(struct m_ExAction *ea, struct m_State *mst)
{
    float eacount = ea->count;
    float stcount = mst->count;
    return eacount / stcount;
}

float SimAgent::CalStatePayoff(struct m_State *mst)
{
    dbgmoreprt("CalStatePayoff(): State: %ld, count: %ld\n", mst->st, mst->count);

    float u0 = mst->original_payoff;
    float pf = u0, tmp;

    struct m_ExAction *ea, *nea;

    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        dbgmoreprt("=============== Ealist ===================\n");
        dbgmoreprt("eid: %ld, count: %ld\n", ea->eat, ea->count);
        tmp = Prob(ea, mst) * MaxPayoffInEat(ea->eat, mst);
        pf += tmp * discount_rate;
        nea = ea->next;
    }
    dbgmoreprt("=============== Ealist End ===================\n");

    return pf;
}

void SimAgent::UpdateState(struct m_State *mst)
{
    /* update state's payoff */
    float payoff = CalStatePayoff(mst);
    dbgmoreprt("UpdateState(): State: %ld, payoff:%.1f\n", mst->st, payoff);

    if (fabsf(mst->payoff - payoff) > threshold)            // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff  = payoff;
        dbgmoreprt("Change to payoff: %.1f\n", payoff);

        /* update backward recursively */
        struct m_BackArcState *bas, *nbas;
        for (bas = mst->blist; bas != NULL; bas = nbas)
        {
            UpdateState(bas->pstate);
            nbas = bas->next;
        }
    }
    else
    {
        dbgmoreprt("Payoff no changes, smaller than %.1f\n", threshold);
    }
    /* update actions' payoff */
    struct m_Action *ac, *nac;
    for (ac=mst->atlist; ac!=NULL; ac=nac)
    {
        ac->payoff = CalActPayoff(ac->act, mst);
        nac = ac->next;
    }

    if (mst->mark == SAVED)
        mst->mark = MODIFIED;           // set mark to modified
    return;
}

struct m_State *SimAgent::StateByEatAct(ExAction eat, Action act, struct m_State *mst)
{
    struct m_ForwardArcState *fas, *nfas;
    for (fas=mst->flist; fas!=NULL; fas=nfas)
    {
        if (fas->eat == eat && fas->act == act)
            return fas->nstate;
        nfas = fas->next;
    }
    return NULL;
}

float SimAgent::CalActPayoff(Action act, struct m_State *mst)
{
    float ori_payoff = 0.0;             // original payoff of actions
    float payoff = ori_payoff;

    struct m_ExAction *ea, *nea;
    struct m_State *nmst;

    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        nmst = StateByEatAct(ea->eat, act, mst);
        if (nmst != NULL)
        {
            payoff += Prob(ea, mst) * (nmst->payoff);
        }
        nea = ea->next;
    }
    dbgmoreprt("CalActPayoff(): state: %ld, act: %ld, payoff:%.1f\n", mst->st, act, payoff);
    return payoff;
}

vector<Action> SimAgent::BestActions(struct m_State *mst, vector<Action> acts)
{
    float max_payoff = -FLT_MAX;
    float ori_payoff = 0.0;         // original payoff of actions
    float payoff;
    vector<Action> max_acts;

    max_acts.clear();
    for (vector<Action>::iterator act = acts.begin();
    act!=acts.end(); ++act)
    {
        struct m_Action *mac = Act2Struct(*act, mst);

        if (mac != NULL)
            payoff = mac->payoff;
        else
            payoff = ori_payoff;

        if (payoff > max_payoff)
        {
            max_acts.clear();
            max_acts.push_back(*act);
            max_payoff = payoff;

        }
        else if (payoff == max_payoff)
            max_acts.push_back(*act);
    }
    return max_acts;
}

void SimAgent::UpdateMemory(float oripayoff, State expst)
{
    if (pre_in == -1)  // previous state doesn't exist, it's the first time
    {
        if (cur_mst == NULL)  // first time without memory, create the state and save it to memory
        {
            cur_mst = NewState(cur_st);
            cur_mst->original_payoff = oripayoff;
            cur_mst->payoff = oripayoff;

            cur_mst->next = head;
            head = cur_mst;

            states_map.insert(StatesMap::value_type(cur_mst->st, cur_mst));          // insert to map
            state_num++;        // update global state number
        }
        else             // state found in memory, simply update its count
        {
            cur_mst->count++;
            if (cur_mst->mark == SAVED)
                cur_mst->mark = MODIFIED;
        }
    }
    else            // previous state exists
    {
        struct m_State *pmst = SearchState(pre_in);
        assert(pmst != NULL);               //ERROR("Previous state lost!\n");

        if (cur_mst == NULL)  // mst not exists, create the state, save it to memory, and link it to the previous state
        {
            cur_mst = NewState(cur_st);
            cur_mst->original_payoff = oripayoff;
            cur_mst->payoff = oripayoff;

            cur_mst->next = head;
            head = cur_mst;
            states_map.insert(StatesMap::value_type(cur_mst->st, cur_mst));
            state_num++;

            ExAction eat = cur_st - expst;              // calcuate exaction
            LinkStates(pmst, eat, pre_out, cur_mst);
        }
        else    // mst already exists, update the count and link it to the previous state (LinkStates will handle it if the link already exists.)
        {
            cur_mst->count++;
            if (cur_mst->mark == SAVED)
                cur_mst->mark = MODIFIED;
            ExAction eat = cur_st - expst;
            LinkStates(pmst, eat, pre_out, cur_mst);
        }
    }
    return;
}

void SimAgent::FreeMemory()
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

void SimAgent::RemoveState(struct m_State *mst)
{
//    dbgprt("remove state\n");
    if (mst == NULL)
        ERROR("Cant remove state, state is NULL\n");
    if (mst->blist != NULL)
    {
        dbgprt("RemoveState(): This state is still linked to other states, do nothing\n");
        return;
    }

    // remove flist recursively
    struct m_ForwardArcState *fas, *nfas;
    for (fas=mst->flist; fas!=NULL; fas=nfas)
    {
        nfas = fas->next;
        RemoveState(fas->nstate);
        FreeFas(fas);
    }

    // remove ealist
    struct m_ExAction *ea, *nea;
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

vector<Action> SimAgent::MaxPayoffRule(State st, vector<Action> acts)
{
    struct m_State *mst = SearchState(st);
    vector<Action> re;

    if (mst == NULL)        // first time to encounter this state, we know nothing about it, so do nothing
    {
        re = acts;
    }
    else                    // we have memory about this state, find the best action of it
    {
//        printf(", Payoff: %.3f\n", mst->payoff);
        re = BestActions(mst, acts);
    }

    cur_mst = mst;
    cur_st = st;
    return re;
}

int SimAgent::GetStateInfo(State st, void *buffer) const
{
    if (buffer == NULL)
    {
        dbgprt("GetStateInfo(): incoming buffer is NULL!\n");
        return -1;
    }

    struct m_State *mst;
    mst = SearchState(st);

    if (mst == NULL)
    {
        dbgprt("GetStateInfo(): State: %ld not found!\n", st);
        return -1;
    }

    unsigned char *ptr = (unsigned char *)buffer;
    struct State_Info_Header stif;
    stif.st = st;
    stif.original_payoff = mst->original_payoff;
    stif.payoff = mst->payoff;
    stif.count = mst->count;

    int act_num = 0;
    int eat_num = 0;
    int lk_num = 0;
    /* Action information */
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
        if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)
        {
            dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
            goto finish;
        }

        nac = ac->next;
    }

    /* ExAction information */
    struct ExAction_Info eaif;
    struct m_ExAction *ea, *nea;
    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        eaif.count = ea->count;
        eaif.eat = ea->eat;
        memcpy(ptr, &eaif, sizeof(struct ExAction_Info));
        ptr += sizeof(struct ExAction_Info);
        eat_num++;
        if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)
        {
            dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
            goto finish;
        }
        nea = ea->next;
    }

    /* links information */
    struct pLink lk;
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
                memcpy(ptr, &lk, sizeof(struct pLink));
                ptr += sizeof(struct pLink);
                lk_num++;
                if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)
                {
                    dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
                    goto finish;
                }
            }
            nfas = fas->next;
        }
        nbas = bas->next;
    }

finish:
    /* create state information with continous space */
    stif.act_num = act_num;
    stif.eat_num = eat_num;
    stif.lk_num = lk_num;

    memcpy(buffer, &stif, sizeof(struct State_Info_Header));
    int length = ptr - (unsigned char *)buffer;
    return length;
}

int SimAgent::MergeStateInfo(struct State_Info_Header *stif)
{
    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    len = stif->eat_num * sizeof(struct ExAction_Info);
    p += len;
    struct pLink *lk = (struct pLink *)p;

    int i;
    struct m_State *mst = SearchState(stif->st);
    int better = 0;         // if sender's info is better than mine

    if (mst == NULL)        // if it's new, copy it in memory
    {
        better = 1;         // anything is better than nothing

        mst = NewState(stif->st);
        /* Add to memory */
        mst->next = head;
        head = mst;
        states_map.insert(StatesMap::value_type(mst->st, mst));
        state_num++;

        mst->payoff = stif->payoff;
        mst->original_payoff = stif->original_payoff;
        mst->count = stif->count;

        /* Actions information */
        for (i=0; i<stif->act_num; i++)
        {
            struct m_Action *mac = (struct m_Action *)malloc(sizeof(struct m_Action));
            mac->act = atif[i].act;
            mac->payoff = atif[i].payoff;

            mac->next = mst->atlist;
            mst->atlist = mac;
        }

        /* ExActions information */
        for (i=0; i<stif->eat_num; i++)
        {
            struct m_ExAction *meat = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));
            meat->eat = eaif[i].eat;
            meat->count = eaif[i].count;

            meat->next = mst->ealist;
            mst->ealist = meat;
        }

        /* links information */
        for (i=0; i<stif->lk_num; i++)
        {
            State pst = lk[i].pst;
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
            better = 0;
            return better;
        }

        better = 1;
        /* stif->count > mst->count */
        mst->payoff = stif->payoff;

        /* actions information */
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
        /* ExActions */
        long delta_count = 0;

        for (i = 0; i < stif->eat_num; i++)
        {
            struct m_ExAction *meat, *nmeat;

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
                struct m_ExAction *neat = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));
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
            State pst = lk[i].pst;
            struct m_State *pmst = SearchState(pst);
            if (pmst != NULL)
            {
                pmst->count++;
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);
            }
        }

    }
    if (better == 1 && mst->mark == SAVED)                // no modification to my state if the recieved one is worse!
        mst->mark = MODIFIED;
    return better;
}

void SimAgent::PrintStateInfo(struct State_Info_Header *stif)
{
    if (stif == NULL)
        return;

    int i;
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
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;
    for (i=0; i<stif->eat_num; i++)
    {
        printf("\t ExAction: %ld,\t\t Count: %ld\n", eaif[i].eat, eaif[i].count);
    }
    printf("------------------------ pLinks, Num: %d ---------------------------\n", stif->lk_num);
    len = stif->eat_num * sizeof(struct ExAction_Info);
    p += len;
    struct pLink *lk = (struct pLink *)p;
    for (i=0; i<stif->lk_num; i++)
    {
        printf("\t pLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", lk[i].pst, lk[i].peat, lk[i].pact);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

void SimAgent::SetDBArgs(string srv, string usr, string passwd, string db)
{
    db_server = srv;
    db_user = usr;
    db_password = passwd;
    db_name = db;
    return;
}

int SimAgent::DBConnect()
{
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
    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, pLinks BLOB) \
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

void SimAgent::DBClose()
{
    return mysql_close(db_con);
}

State SimAgent::DBStateByIndex(unsigned long index)
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", db_t_stateinfo.c_str(), index);

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBStateByIndex(): result == NULL!\n");
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBStateByIndex(): lengths is null\n");
        mysql_free_result(result);
        return -1;
    }
    State rs = atol(row[0]);

    mysql_free_result(result);          // free result
    return rs;
}

int SimAgent::DBFetchStateInfo(State st, void *buffer)
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBFetchStateInfo(): result == NULL!\n");
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        dbgmoreprt("DBFetchStateInfo(): Fields don't match!\n");
        return -1;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBFetchStateInfo(): lengths is null\n");
        return -1;
    }

    unsigned char *ptr = (unsigned char *)buffer;

    unsigned long ai_len = lengths[4];
    unsigned long ea_len = lengths[5];
    unsigned long lk_len = lengths[6];

    struct State_Info_Header stif;
    stif.st = atol(row[0]);
    stif.original_payoff = atof(row[1]);
    stif.payoff = atof(row[2]);
    stif.count = atol(row[3]);

    stif.act_num = ai_len / sizeof(struct Action_Info);
    stif.eat_num = ea_len / sizeof(struct ExAction_Info);
    stif.lk_num = lk_len / sizeof(struct pLink);

    memcpy(ptr, &stif, sizeof(struct State_Info_Header));

    ptr += sizeof(struct State_Info_Header);
    memcpy(ptr, row[4], ai_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)
    {
        dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    ptr += ai_len;
    memcpy(ptr, row[5], ea_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)          // CHECK NEEDED
    {
        dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    ptr += ea_len;
    memcpy(ptr, row[6], lk_len);
    if ((ptr - (unsigned char *)buffer) > SI_MAX_SIZE)
    {
        dbgprt("WARNNING: StateInfo size exceeds SI_MAX_SIZE!\n");
        goto finish;
    }

    mysql_free_result(result);          // free result

finish:
    int length = ptr - (unsigned char *)buffer;
    return length;
}

int SimAgent::DBSearchState(State st)
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

void SimAgent::DBAddStateInfo(struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str, "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, pLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
            db_t_stateinfo.c_str(), stif->st, stif->original_payoff, stif->payoff, stif->count);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    p += ea_len;
    struct pLink *lk = (struct pLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(db_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(db_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(db_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

//    dbgprt("query: %s\n", query);
    if (mysql_real_query(db_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

void SimAgent::DBUpdateStateInfo(struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str, "UPDATE %s SET OriPayoff=%.2f, Payoff=%.2f, Count=%ld, ActInfos='%%s', ExActInfos='%%s', pLinks='%%s' WHERE State=%ld",
            db_t_stateinfo.c_str(), stif->original_payoff, stif->payoff, stif->count, stif->st);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    p += ea_len;
    struct pLink *lk = (struct pLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(db_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(db_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(db_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

    if (mysql_real_query(db_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

void SimAgent::DBDeleteState(State st)
{
    char query_string[256];
    sprintf(query_string, "DELETE  FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

void SimAgent::DBAddMemoryInfo()
{
    char query_str[256];

    sprintf(query_str, "INSERT INTO %s(TimeStamp, DiscountRate, Threshold, NumStates, NumLinks, LastState, LastAction) VALUES(NULL, %.2f, %.2f, %ld, %ld, %ld, %ld)",
            db_t_meminfo.c_str(), discount_rate, threshold, state_num, lk_num, pre_in, pre_out);

    int len = strlen(query_str);
    if (mysql_real_query(db_con, query_str, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

struct m_Memory_Info *SimAgent::DBFetchMemoryInfo()
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s ORDER BY TimeStamp DESC LIMIT 1", db_t_meminfo.c_str());

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("DBFetchMemoryInfo(): result == NULL!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBFetchMemoryInfo(): lengths is null\n");
        mysql_free_result(result);
        return NULL;
    }

    struct m_Memory_Info *memif = (struct m_Memory_Info *)malloc(sizeof(struct m_Memory_Info));
    dbgprt("DB: %s, Memory TimeStamp: %s\n", db_name.c_str(), row[0]);
    memif->discount_rate = atof(row[1]);
    memif->threshold = atof(row[2]);
    memif->state_num = atol(row[3]);
    memif->lk_num = atol(row[4]);
    memif->last_st = atol(row[5]);
    memif->last_act = atol(row[6]);

    mysql_free_result(result);          // free result

    return memif;
}
