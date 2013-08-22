/**********************************************************************
*	@File:
*	@Created: 2013-8-19
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "MyAgent.h"

void MyAgent::LoadMemory()
{
    return;
}

void MyAgent::DumpMemory()
{
    return;
}

MyAgent::MyAgent(int n, int m):Agent(n, m)
{
    //ctor
    mem_file = "MyAgent.mem";
    threshold = 0.01;

    state_num = arc_num = 0;
    head = NULL;

    LoadMemory();
}

MyAgent::MyAgent(int n, int m, float dr):Agent(n, m, dr)
{
    mem_file = "MyAgent.mem";
    threshold = 0.01;

    state_num = arc_num = 0;
    head = NULL;

    LoadMemory();
}

MyAgent::MyAgent(int n, int m, float dr, float th, string memfile):Agent(n, m, dr)
{
    mem_file = memfile;
    threshold = th;

    state_num = arc_num = 0;
    head = NULL;

    LoadMemory();
}

MyAgent::~MyAgent()
{
    //dtor
    DumpMemory();
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

    return FreeState(mst);
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

    arc_num++;       // update total arc number
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
    dbgprt("UpdateState(): mstate: %d, payoff:%.1f\n", mst->st, payoff);

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
    dbgprt("CalActPayoff(): state: %d, act: %d, payoff:%.1f\n", mst->st, act, payoff);
    return payoff;
}

vector<Action> MyAgent::BestActions(struct m_State *mst, vector<Action> acts)
{
    printf("BestActions()============\n");

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
    printf("Exit BestActions()...........\n");

    return max_acts;
}

void MyAgent::SaveState(struct m_State *mst, State st)
{
    printf("SaveState() ...........\n");

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
            ExAction eat = Agent::CalExAction(pre_in, st, pre_out);
            LinkStates(pmst, eat, pre_out, mst);
        }
    }
    printf("Exit SaveState() ...........\n");

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
    printf("MaxPayoffRule(): State: %d\n", st);
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
    printf("Exit MaxPayoffRule()...........\n");

    return re;
}

struct State_Info *MyAgent::GetStateInfo(State st)
{
    struct m_State *mst;
    mst = SearchState(st);

    if (mst == NULL)
    {
        dbgprt("State: %d not found!\n", st);
        return NULL;
    }

    /* Action information */
    int ai_len = 0;
    int max_len = 50;
    struct Action_Info tmp_atif[max_len];

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
            tmp_atif = realloc(tmp_atif, max_len);

            tmp_atif[ai_len].act = act;
            tmp_atif[ai_len].payoff = payoff;
        }
        ai_len++;
        nac = ac->next;
    }

    /* ExAction information */
    int ea_len = 0;
    max_len = 50;
    struct ExAction_Info tmp_etif[max_len];

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
            tmp_etif = realloc(tmp_etif, max_len);

            tmp_etif[ea_len].eat = ea->eat;
            tmp_etif[ea_len].count = ea->count;
        }
        ea_len++;
        nea = ea->next;
    }

    /* links information */
    int lk_len = 0;
    max_len = 50;
    struct pLink tmp_lk[max_len];

    struct m_BackArcState *bas, *nbas;
    for (bas=mst->blist; bas!=NULL; bas=nbas)
    {
        struct m_State *pmst = SearchState(plk.pst);
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
                    tmp_lk = realloc(tmp_lk, max_len);

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


    struct State_Info *stif = (struct State_Info *)malloc(st_size);
    stif->st = mst->st;
    stif->original_payoff = mst->original_payoff;
    stif->payoff = mst->payoff;
    stif->count = mst->count;
    stif->length = st_size;
    stif->actions_info = vai;   // seg fault
    stif->belief = vbf;         // memmove() segement fault
    stif->plinks = vlk;

    return stif;                // raise() Abort
}

int MyAgent::MergeStateInfo(struct State_Info *stif)
{
    printf("MergeStateInfo ....:\n");

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
        for (vector<struct Action_Info>::iterator acif = stif->actions_info.begin();
        acif != stif->actions_info.end(); ++acif)
        {
            struct m_Action *mac = (struct m_Action *)malloc(sizeof(struct m_Action));
            mac->act = (*acif).act;
            mac->payoff = (*acif).payoff;

            mac->next = mst->atlist;
            mst->atlist = mac;
        }

        /* ExActions information */
        for (vector<struct ExAction_Info>::iterator exif = stif->belief.begin();
        exif != stif->belief.end(); ++exif)
        {
            struct m_ExAction *meat = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));
            meat->eat = (*exif).eat;
            meat->count = (*exif).count;

            meat->next = mst->ealist;
            mst->ealist = meat;
        }

        /* links information */
        for (vector<struct pLink>::iterator link = stif->plinks.begin();
        link != stif->plinks.end(); ++link)
        {
            State pst = (*link).pst;
            struct m_State *pmst = SearchState(pst);            // find if the previous state exists
            if (pmst != NULL)                                   // if so, make the link, otherwise do nothing
                LinkStates(pmst, (*link).peat, (*link).pact, mst);
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
        for (vector<struct Action_Info>::iterator acif = stif->actions_info.begin();        // for every action recieved, compare it with my own version
        acif != stif->actions_info.end(); ++acif)
        {
            struct m_Action *mac, *nmac;
            for (mac=mst->atlist; mac!=NULL; mac=nmac)
            {
                if ((mac->act == (*acif).act) && (mac->payoff < (*acif).payoff))  // choose the bigger
                {
                    mac->payoff = (*acif).payoff;
                    better = 1;
                    break;                                                        // one action should occur only once, break if we found one
                }
                nmac = mac->next;
            }

            if (mac == NULL)            // no corresponding action found in my own state, it's a new one, create it.
            {
                better = 1;
                struct m_Action *nmac = (struct m_Action *)malloc(sizeof(struct m_Action));
                nmac->act = (*acif).act;
                nmac->payoff = (*acif).payoff;

                nmac->next = mst->atlist;
                mst->atlist = nmac;
            }
        }

        mst->count += stif->count;       // Add up the counts of state
        /* ExActions */
        for (vector<struct ExAction_Info>::iterator exif = stif->belief.begin();
        exif != stif->belief.end(); ++exif)
        {
            struct m_ExAction *meat, *nmeat;
            for (meat=mst->ealist; meat!=NULL; meat=nmeat)
            {
                if (meat->eat == (*exif).eat) // add up exact counts
                {
                    meat->count += (*exif).count;
                    break;
                }
                nmeat = meat->next;
            }

            // new eat, create one
            if (meat == NULL)
            {
                better = 1;
                struct m_ExAction *neat = (struct m_ExAction *)malloc(sizeof(struct m_ExAction));
                neat->eat = (*exif).eat;
                neat->count = (*exif).count;

                neat->next = mst->ealist;
                mst->ealist = neat;
            }
        }

        /* links, make the link if previous state exists */
        for (vector<struct pLink>::iterator link = stif->plinks.begin();
        link != stif->plinks.end(); ++link)
        {
            State pst = (*link).pst;
            struct m_State *pmst = SearchState(pst);
            if (pmst != NULL)
                LinkStates(pmst, (*link).peat, (*link).pact, mst);
        }

    }
    printf("exit MergeStateInfo==============\n");
    return better;
}

void MyAgent::PrintStateInfo(struct State_Info *stif)
{
    printf("===================== State: %ld =========================\n", stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n", stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions Info -----------------------\n");
    for (vector<struct Action_Info>::iterator atif = stif->actions_info.begin();
    atif != stif->actions_info.end(); ++atif)
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", (*atif).act, (*atif).payoff);
    printf("---------------------- pLink Info ------------------------\n");
    for (vector<struct pLink>::iterator plif = stif->plinks.begin();
    plif != stif->plinks.end(); ++plif)
        printf("\t pLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", (*plif).pst, (*plif).pact, (*plif).peat);
    printf("-------------------- ExActions Info ----------------------\n");
    for (vector<struct ExAction_Info>::iterator eaif = stif->belief.begin();
    eaif != stif->belief.end(); ++eaif)
        printf("\t ExAction: %ld,\t\t Count: %ld\n", (*eaif).eat, (*eaif).count);
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}
