/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Avatar.h"

int Avatar::quit = 0;

Avatar::Avatar(int i)
{
    //ctor
    id = i;
    freq = 100;
    agent = NULL;
    group = NULL;
    pre_st = -1;
    pre_act = -1;
}

Avatar::~Avatar()
{
    //dtor
}

pthread_t Avatar::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}

void Avatar::Run()
{
    int count = 0;
    while(!quit)
    {
        RecvStateInfo();

        State cs = GetCurrentState();
        printf("Id: %d, Current state: %ld\n", id, cs);
        vector<Action> acts = ActionList(cs);

        Action act = agent->Process(cs, acts);

        float oripayoff = OriginalPayoff(cs);
        State es = ExpectedState();
        agent->Update(oripayoff, es);

        if (act == INVALID_VALUE)
            break;
        DoAction(act);

        pre_st = cs;
        pre_act = act;

        if (count >= freq)
        {
            SendStateInfo(cs);
            count = 0;
        }
        else
            count++;
    }
    dbgmoreprt("Id: %d, Run() Exit!\n", id);
    return;
}

void Avatar::SetFreq(int fq)
{
    freq = fq;
    return;
}

void Avatar::ConnectAgent(Agent *agt)
{
    agent = agt;
    return;
}

void Avatar::JoinGroup(Group *grp)
{
    group = grp;
    return;
}

void Avatar::SendStateInfo(State st)
{
    if (group == NULL)
        return;

    char si_buffer[SI_MAX_SIZE];
    int len = agent->GetStateInfo(st, si_buffer);       // the st may not exist
    if (len == -1)
    {
        return;
    }

    group->Send(id, si_buffer, len);

    return;
}

void Avatar::RecvStateInfo()
{
    if (group == NULL)
        return;

    char re_buf[SI_MAX_SIZE];
    char sd_buf[SI_MAX_SIZE];

    while(group->Recv(id, re_buf, 2048) != 0)
    {
        struct State_Info_Header *stif = (struct State_Info_Header *)re_buf;
        int better = agent->MergeStateInfo(stif);
        if (better == 0)                // send out my information if it is not better
        {
            int len = agent->GetStateInfo(stif->st, sd_buf);
            if (len != -1)
            {
                group->Send(id, sd_buf, len);
            }
        }
    }
    return;
}

/** \brief Get original payoff of each state.
 *  Return 1 for every state.
 * \param st state identity
 * \return original payoff of st
 *
 */

float Avatar::OriginalPayoff(State st)
{
    UNUSED(st);
    return 1.0;
}

