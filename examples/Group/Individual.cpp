/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Individual.h"

Individual::Individual(int i)
{
    id = i;
    signal(SIGUSR1, SignalHandler);
}

Individual::~Individual()
{
    //dtor
}

void Individual::SendStateInfo()
{
    struct State_Info *si = agent.GetStateInfo(current_state);       // can be NULL
    if (si == NULL)
        return;

    SimGroup::Send(id, si, si->length);

    free(si);           // freed??
    return;
}


void Individual::RecvStateInfo()
{
    char buf[1028];

    while(SimGroup::Recv(id, buf, 1028) != 0)
    {
        struct State_Info *stif = (struct State_Info *)buf;

        int better = agent.MergeStateInfo(stif);
        if (better == 0)                // send out my information if it is better
        {
            struct State_Info *stif = agent.GetStateInfo(stif->st);
            SimGroup::Send(id, stif, stif->length);
            free(stif);               // freed?
        }
    }
    return;
}

void Individual::SetFreq(int fq)
{
    freq = fq;
    return;
}

int Individual::GetID()
{
    return id;
}

void Individual::SignalHandler(int sig)
{
    if (sig == SIGUSR1)
        RecvStateInfo();
    return;
}

void Individual::Run()
{
    int count = 0;
    while(1)
    {
        State cs = GetCurrentState();
        current_state = cs;
        Action act = agent.Process(cs);
        if (act == -1)
            break;
        DoAction(act);

        if (count >= freq)
        {
            SendStateInfo();
            count = 0;
        }
        else
            count++;
    }
    return;
}
