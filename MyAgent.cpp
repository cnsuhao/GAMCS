/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "MyAgent.h"

struct m_State *MyAgent::LoadState(State st)
{
    struct m_State *mst = SearchState(st);
    if (mst == NULL)
    {
        struct State_Info *stif = DBFetchStateInfo(st);
        if (stif == NULL)       // should not happen, database conrrupted!!
            return NULL;
        mst = NewState(st);

        mst->mark = SAVED;      // it's SAVED when just load
        mst->st = stif->st;
        mst->original_payoff = stif->original_payoff;
        mst->payoff = stif->payoff;
        mst->count = stif->count;
        /* Add to memory */
        mst->next = head;
        head = mst;

        unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
        unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);

        unsigned char *p = (unsigned char *)stif;
        p += sizeof(struct State_Info);
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

        free(stif);
    }
    return mst;
}

void MyAgent::InitMemory()
{
    if (db_name.empty())
        return;

    dbgprt("Initializing Memory...\n");
    int re = DBConnect();
    if (re == 0)
    {
        /* load memory information */
        struct m_Memory_Info *memif = DBFetchMemoryInfo();
        if (memif != NULL)
        {
            N = memif->N;
            M = memif->M;
            discount_rate = memif->discount_rate;
            threshold = memif->threshold;
            state_num = memif->state_num;
            lk_num = memif->lk_num;
            free(memif);
        }

        /* load states information */
        State st;
        vector<unsigned long> states;
        states.clear();
        while((st = DBNextState()) != -1)
        {
           printf("State: %ld\n", st);
            states.push_back(st);
        }

        for (vector<unsigned long>::iterator it = states.begin(); it!=states.end(); ++it)
        {
            LoadState(*it);
        }
    }

    DBClose();
    return;
}

void MyAgent::SaveMemory()
{
    dbgprt("Saving Memory...\n");
    int re = DBConnect();
    if (re == 0)
    {
        /* save memory information */
        DBAddMemoryInfo();

        /* save states information */
        struct m_State *mst, *nmst;
        for (mst=head; mst!=NULL; mst=nmst)
        {
            if (mst->mark == NEW)
            {
                struct State_Info *stif = GetStateInfo(mst->st);
                DBAddStateInfo(stif);
                free(stif);
            }
            else if (mst->mark == MODIFIED)
            {
                struct State_Info *stif = GetStateInfo(mst->st);
                DBUpdateStateInfo(stif);
                free(stif);
            }
            mst->mark = SAVED;

            nmst = mst->next;
        }
    }

    DBClose();
    return;
}

MyAgent::MyAgent(int n, int m):Agent(n, m)
{
    //ctor
    threshold = 0.01;

    state_num = lk_num = 0;
    head = NULL;

    db_con = NULL;
    db_server = "";
    db_user = "";
    db_password = "";
    db_name = "";
    db_t_stateinfo = "StateInfo";
    db_t_meminfo = "MemoryInfo";
}

MyAgent::MyAgent(int n, int m, float dr, float th):Agent(n, m, dr)
{
    threshold = th;

    state_num = lk_num = 0;
    head = NULL;

    db_con = NULL;
    db_server = "";
    db_user = "";
    db_password = "";
    db_name = "";
    db_t_stateinfo = "StateInfo";
    db_t_meminfo = "MemoryInfo";
}

MyAgent::~MyAgent()
{
    //dtor
    if (!db_name.empty())
        SaveMemory();
    FreeMemory();
}

struct m_State *MyAgent::SearchState(State st)
{
    struct m_State *mst, *nmst;

    for (mst = head; mst != NULL; mst = nmst)
    {
        if (mst->st == st)
            return mst;

        nmst = mst->next;
    }

    return NULL;
}

struct m_State *MyAgent::NewState(State st)
{
    struct m_State *mst = (struct m_State *)malloc(sizeof(struct m_State));
    mst->st = st;
    mst->original_payoff = OriginalPayoff(st);
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

void MyAgent::FreeState(struct m_State *mst)
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

struct m_ForwardArcState *MyAgent::NewFas(ExAction eat, Action act)
{
    struct m_ForwardArcState *fas = (struct m_ForwardArcState *)malloc(sizeof(struct m_ForwardArcState));

    fas->act = act;
    fas->eat = eat;
    fas->nstate = NULL;
    fas->next = NULL;
    return fas;
}

void MyAgent::FreeFas(struct m_ForwardArcState *fas)
{
    return free(fas);
}

struct m_BackArcState *MyAgent::NewBas()
{
    struct m_BackArcState *bas = (struct m_BackArcState *)malloc(sizeof(struct m_BackArcState));
    bas->pstate = NULL;
    bas->next = NULL;
    return bas;
}

void MyAgent::FreeBas(struct m_BackArcState *bas)
{
    return free(bas);
}

struct m_Action* MyAgent::Act2Struct(Action act, struct m_State *mst)
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

struct m_ExAction* MyAgent::Eat2Struct(ExAction eat, struct m_State *mst)
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

struct m_ExAction *MyAgent::NewEa(ExAction eat)
{
    struct m_ExAction *ea = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));

    ea->eat = eat;
    ea->count = 1;
    ea->next = NULL;
    return ea;
}

void MyAgent::FreeEa(struct m_ExAction *ea)
{
    return free(ea);
}

struct m_Action *MyAgent::NewAc(Action act)
{
    struct m_Action *ac = (struct m_Action *)malloc(sizeof(struct m_Action));

    ac->act = act;
    ac->payoff = 0;
    ac->next = NULL;
    return ac;
}

void MyAgent::FreeAc(struct m_Action *ac)
{
    return free(ac);
}

void MyAgent::LinkStates(struct m_State *pmst, ExAction eat, Action act, struct m_State *mst)
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

float MyAgent::MaxPayoffInEat(ExAction eat, struct m_State *mst)
{
    float max_pf = -999999.0;
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

float MyAgent::Prob(struct m_ExAction *ea, struct m_State *mst)
{
    float eacount = ea->count;
    float stcount = mst->count;
    return eacount / stcount;
}

float MyAgent::CalStatePayoff(struct m_State *mst)
{
    float u0 = mst->original_payoff;
    float pf = u0, tmp;

    struct m_ExAction *ea, *nea;

    for (ea = mst->ealist; ea != NULL; ea = nea)
    {
        tmp = Prob(ea, mst) * MaxPayoffInEat(ea->eat, mst);
        pf += tmp * discount_rate;
        nea = ea->next;
    }

    return pf;
}

void MyAgent::UpdateState(struct m_State *mst)
{
    /* update state's payoff */
    float payoff = CalStatePayoff(mst);
    dbgprt("UpdateState(): mstate: %ld, payoff:%.1f\n", mst->st, payoff);

    if (fabsf(mst->payoff - payoff) > threshold)            // compare with threshold, update if the diff exceeds threshold
    {
        mst->payoff  = payoff;
        dbgprt("To payoff: %.1f\n", payoff);

        /* update backward recursively */
        struct m_BackArcState *bas, *nbas;
        for (bas = mst->blist; bas != NULL; bas = nbas)
        {
            UpdateState(bas->pstate);
            nbas = bas->next;
        }
    }
    else
        dbgprt("Payoff no changes, smaller than %.1f\n", threshold);

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

struct m_State *MyAgent::StateByEatAct(ExAction eat, Action act, struct m_State *mst)
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

float MyAgent::CalActPayoff(Action act, struct m_State *mst)
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
    dbgprt("CalActPayoff(): state: %ld, act: %ld, payoff:%.1f\n", mst->st, act, payoff);
    return payoff;
}

vector<Action> MyAgent::BestActions(struct m_State *mst, vector<Action> acts)
{
    float max_payoff = -999999999.9;
    float ori_payoff = 0.0;
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

void MyAgent::SaveState(struct m_State *mst, State st)
{
    if (pre_in == -1)  // previous state doesn't exist, it's the first time
    {
        if (mst == NULL)  // first time without memory, create the state and save it to memory
        {
            mst = NewState(st);

            mst->next = head;
            head = mst;
            state_num++;        // update global state number
        }
        else             // state found in memory, simply update its count
        {
            mst->count++;
            if (mst->mark == SAVED)
                mst->mark = MODIFIED;
        }
    }
    else            // previous state exists
    {
        struct m_State *pmst = SearchState(pre_in);
        if (pmst == NULL)
            ERROR("Previous state lost!\n");

        if (mst == NULL)  // mst not exists, create the state, save it to memory, and link it to the previous state
        {
            mst = NewState(st);

            mst->next = head;
            head = mst;
            state_num++;

            ExAction eat = Agent::CalExAction(pre_in, st, pre_out);
            LinkStates(pmst, eat, pre_out, mst);
        }
        else    // mst already exists, update the count and link it to the previous state (LinkStates will handle it if the link already exists.)
        {
            mst->count++;
            if (mst->mark == SAVED)
                mst->mark = MODIFIED;
            ExAction eat = Agent::CalExAction(pre_in, st, pre_out);
            LinkStates(pmst, eat, pre_out, mst);
        }
    }
    return;
}

void MyAgent::FreeMemory()
{
    // free all states in turn
    struct m_State *mst, *nmst;
    for (mst=head; mst!=NULL; mst=nmst)
    {
        nmst = mst->next;
        FreeState(mst);
    }
}

void MyAgent::RemoveState(struct m_State *mst)
{
    dbgprt("remove state\n");
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

vector<Action> MyAgent::MaxPayoffRule(State st, vector<Action> acts)
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

    SaveState(mst, st);     // save the state to our memory
    return re;
}

struct State_Info *MyAgent::GetStateInfo(State st)
{
    struct m_State *mst;
    mst = SearchState(st);

    if (mst == NULL)
    {
        dbgprt("State: %ld not found!\n", st);
        return NULL;
    }

    /* Action information */
    int ai_len = 0;
    int max_len = 50;
    struct Action_Info *tmp_atif = (struct Action_Info *)malloc(max_len*sizeof(struct Action_Info));

    struct m_Action *ac, *nac;
    for (ac=mst->atlist; ac!=NULL; ac=nac)
    {
        Action act = ac->act;
        float payoff = ac->payoff;
        if (ai_len < max_len)
        {
            tmp_atif[ai_len].act = act;
            tmp_atif[ai_len].payoff = payoff;
        }
        else
        {
            max_len += 50;
            tmp_atif = (struct Action_Info *)realloc(tmp_atif, max_len*sizeof(struct Action_Info));

            tmp_atif[ai_len].act = act;
            tmp_atif[ai_len].payoff = payoff;
        }
        ai_len++;
        nac = ac->next;
    }

    /* ExAction information */
    int ea_len = 0;
    max_len = 50;
    struct ExAction_Info *tmp_etif = (struct ExAction_Info *)malloc(max_len*sizeof(struct ExAction_Info));

    struct m_ExAction *ea, *nea;
    for (ea=mst->ealist; ea!=NULL; ea=nea)
    {
        if (ea_len < max_len)
        {
            tmp_etif[ea_len].eat = ea->eat;
            tmp_etif[ea_len].count = ea->count;
        }
        else
        {
            max_len += 50;
            tmp_etif = (struct ExAction_Info *)realloc(tmp_etif, max_len*sizeof(struct ExAction_Info));

            tmp_etif[ea_len].eat = ea->eat;
            tmp_etif[ea_len].count = ea->count;
        }
        ea_len++;
        nea = ea->next;
    }

    /* links information */
    int lk_len = 0;
    max_len = 50;
    struct pLink *tmp_lk = (struct pLink *)malloc(max_len*sizeof(struct pLink));

    struct m_BackArcState *bas, *nbas;
    for (bas=mst->blist; bas!=NULL; bas=nbas)
    {
        struct m_State *pmst = SearchState(bas->pstate->st);
        if (pmst == NULL)
            ERROR("GetStateInfo(): Memory conrupt!\n");
        struct m_ForwardArcState *fas, *nfas;
        for (fas=pmst->flist; fas!=NULL; fas=nfas)
        {
            if (fas->nstate->st == mst->st)
            {
                if (lk_len < max_len)
                {
                    tmp_lk[lk_len].pst = bas->pstate->st;
                    tmp_lk[lk_len].peat = fas->eat;
                    tmp_lk[lk_len].pact = fas->act;
                }
                else
                {
                    max_len += 50;
                    tmp_lk = (struct pLink *)realloc(tmp_lk, max_len*sizeof(struct pLink));

                    tmp_lk[lk_len].pst = bas->pstate->st;
                    tmp_lk[lk_len].peat = fas->eat;
                    tmp_lk[lk_len].pact = fas->act;
                }
                lk_len++;
            }
            nfas = fas->next;
        }
        nbas = bas->next;
    }

    /* create state information with continous space */
    int len = sizeof(struct State_Info) + ai_len*sizeof(struct Action_Info) +\
                ea_len*sizeof(struct ExAction_Info) + lk_len*sizeof(struct pLink);

    struct State_Info *stif = (struct State_Info *)malloc(len);
    stif->st = mst->st;
    stif->original_payoff = mst->original_payoff;
    stif->payoff = mst->payoff;
    stif->count = mst->count;
    stif->length = len;
    stif->act_num = ai_len;
    stif->eat_num = ea_len;
    stif->lk_num = lk_len;

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    len = ai_len * sizeof(struct Action_Info);
    memcpy(p, tmp_atif, len);
    free(tmp_atif);

    p += len;
    len = ea_len * sizeof(struct ExAction_Info);
    memcpy(p, tmp_etif, len);
    free(tmp_etif);

    p += len;
    len = lk_len * sizeof(struct pLink);
    memcpy(p, tmp_lk, len);
    free(tmp_lk);

    return stif;
}


int MyAgent::MergeStateInfo(struct State_Info *stif)
{
    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
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
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);

        }
    }
    else            // state already exists, merge the recieved one with it
    {
        if (stif->payoff > mst->payoff)     // choose the bigger one
        {
            mst->payoff = stif->payoff;
            better = 1;                     // mark it better
        }

        /* actions information */
        for (i=0; i<stif->act_num; i++)
        {
            struct m_Action *mac, *nmac;
            for (mac=mst->atlist; mac!=NULL; mac=nmac)
            {
                if ((mac->act == atif[i].act))
                {
                    if (mac->payoff < atif[i].payoff)       // choose the bigger
                    {
                        mac->payoff = atif[i].payoff;
                        better = 1;
                    }
                    break;                 // one action should occur only once, break if we found one
                }
                nmac = mac->next;
            }

            if (mac == NULL)            // no corresponding action found in my own state, it's a new one, create it.
            {
                better = 1;
                struct m_Action *nmac = (struct m_Action *)malloc(sizeof(struct m_Action));
                nmac->act = atif[i].act;
                nmac->payoff = atif[i].payoff;

                nmac->next = mst->atlist;
                mst->atlist = nmac;
            }
        }

        /* counts */
        if (mst->count < stif->count)       // more experienced
        {
            better = 1;

            mst->count = stif->count;       // state count

            /* ExActions */
            for (i=0; i<stif->eat_num; i++)
            {
                struct m_ExAction *meat, *nmeat;
                for (meat=mst->ealist; meat!=NULL; meat=nmeat)
                {
                    if (meat->eat == eaif[i].eat)
                    {
                        meat->count = eaif[i].count;        // use the recieved eat count
                        break;
                    }
                    nmeat = meat->next;
                }

                // new eat, create one
                if (meat == NULL)
                {
                    better = 1;
                    struct m_ExAction *neat = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));
                    neat->eat = eaif[i].eat;
                    neat->count = eaif[i].count;

                    neat->next = mst->ealist;
                    mst->ealist = neat;
                }
            }
        } //if

        /* links, make the link if previous state exists */
        for (i=0; i<stif->lk_num; i++)
        {
            State pst = lk[i].pst;
            struct m_State *pmst = SearchState(pst);
            if (pmst != NULL)
                LinkStates(pmst, lk[i].peat, lk[i].pact, mst);
        }

        if (better == 1 && mst->mark == SAVED)                // no modification to my state if the recieved one is worse!
            mst->mark = MODIFIED;
    }
    return better;
}

void MyAgent::PrintStateInfo(struct State_Info *stif)
{
    int i;
    printf("===================== State: %ld =========================\n", stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n", stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions, Num: %d -----------------------\n", stif->act_num);
    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
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

void MyAgent::SetDBArgs(string srv, string usr, string passwd, string db)
{
    db_server = srv;
    db_user = usr;
    db_password = passwd;
    db_name = db;
    return;
}

int MyAgent::DBConnect()
{
    db_con = mysql_init(NULL);

    if (db_con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    if (mysql_real_connect(db_con, db_server.c_str(), db_user.c_str(), db_password.c_str(), db_name.c_str(),
                           0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    /* create table if not exists */
    char tb_string[256];
    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, pLinks BLOB) \
            ENGINE INNODB ", db_name.c_str(), db_t_stateinfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(TimeStamp TIMESTAMP PRIMARY KEY, N BIGINT, M BIGINT, DiscountRate FLOAT, Threshold FLOAT, NumStates BIGINT, NumLinks BIGINT) \
            ENGINE INNODB ", db_name.c_str(), db_t_meminfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    return 0;
}

void MyAgent::DBClose()
{
    return mysql_close(db_con);
}

State MyAgent::DBNextState()
{
    static unsigned long offset = 0;
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", db_t_stateinfo.c_str(), offset);

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        printf("no result!\n");
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        return -1;
    }
    State rs = atol(row[0]);
    offset++;

    mysql_free_result(result);          // free result
    return rs;
}

struct State_Info *MyAgent::DBFetchStateInfo(State st)
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        printf("no result!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        printf("Fields don't match!\n");
        return NULL;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        printf("lengths is null\n");
        return NULL;
    }

    unsigned long total_len = 0;

    unsigned long ai_len = lengths[4];
    unsigned long ea_len = lengths[5];
    unsigned long lk_len = lengths[6];
    total_len = sizeof(unsigned long)*2 + sizeof(float)*2 + sizeof(int)*4 + ai_len + ea_len + lk_len;

    struct State_Info *stif = (struct State_Info *)malloc(total_len);
    stif->st = atol(row[0]);
    stif->original_payoff = atof(row[1]);
    stif->payoff = atof(row[2]);
    stif->count = atol(row[3]);
    stif->length = total_len;

    stif->act_num = ai_len / sizeof(struct Action_Info);
    stif->eat_num = ea_len / sizeof(struct ExAction_Info);
    stif->lk_num = lk_len / sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    memcpy(p, row[4], ai_len);

    p += ai_len;
    memcpy(p, row[5], ea_len);

    p += ea_len;
    memcpy(p, row[6], lk_len);

    mysql_free_result(result);          // free result
    return stif;
}

int MyAgent::DBSearchState(State st)
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

void MyAgent::DBAddStateInfo(struct State_Info *stif)
{
    dbgprt("DBAddStateInfo.....\n");
    char str[256];
    sprintf(str, "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, pLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
            db_t_stateinfo.c_str(), stif->st, stif->original_payoff, stif->payoff, stif->count);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
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

    dbgprt("query: %s\n", query);
    if (mysql_real_query(db_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

void MyAgent::DBUpdateStateInfo(struct State_Info *stif)
{
    dbgprt("DBUpdateStateInfo.....\n");
    char str[256];
    sprintf(str, "UPDATE %s SET OriPayoff=%.2f, Payoff=%.2f, Count=%ld, ActInfos='%%s', ExActInfos='%%s', pLinks='%%s' WHERE State=%ld",
            db_t_stateinfo.c_str(), stif->original_payoff, stif->payoff, stif->count, stif->st);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
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

    dbgprt("query: %s\n", query);

    if (mysql_real_query(db_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

void MyAgent::DBDeleteState(State st)
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

void MyAgent::DBAddMemoryInfo()
{
    dbgprt("DBAddMemoryInfo.....\n");
    char query_str[256];

    sprintf(query_str, "INSERT INTO %s(TimeStamp, N, M, DiscountRate, Threshold, NumStates, NumLinks) VALUES(NULL, %ld, %ld, %.2f, %.2f, %ld, %ld)",
            db_t_meminfo.c_str(), N, M, discount_rate, threshold, state_num, lk_num);

    int len = strlen(query_str);
    if (mysql_real_query(db_con, query_str, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

struct m_Memory_Info *MyAgent::DBFetchMemoryInfo()
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
        printf("no result!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        printf("lengths is null\n");
        return NULL;
    }

    struct m_Memory_Info *memif = (struct m_Memory_Info *)malloc(sizeof(struct m_Memory_Info));
    dbgprt("Memory TimeStamp: %s\n", row[0]);
    memif->N = atol(row[1]);
    memif->M = atol(row[2]);
    memif->discount_rate = atof(row[3]);
    memif->threshold = atof(row[4]);
    memif->state_num = atol(row[5]);
    memif->lk_num = atol(row[6]);

    mysql_free_result(result);          // free result

    return memif;
}
