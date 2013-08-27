/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Avatar.h"
#include "MyAgent.h"

int Avatar::quit = 0;

Avatar::Avatar(int i)
{
    //ctor
    id = i;
    freq = 100;
    agent = NULL;
    group = NULL;
}

Avatar::~Avatar()
{
    //dtor
}

int Avatar::ThreadRun()
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
        Action act = agent->Process(cs);
        if (act == -1)
            break;
        DoAction(act);

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

void Avatar::SetAgent(Agent *agt)
{
    agent = agt;
    return;
}

void Avatar::SetGroup(Group *grp)
{
    group = grp;
    return;
}

void Avatar::SendStateInfo(State st)
{
    if (group == NULL)
        return;

    struct State_Info *si = agent->GetStateInfo(st);       // can be NULL
    if (si == NULL)
    {
        dbgmoreprt("si == NULL\n");
        return;
    }

    group->Send(id, si, si->length);

    free(si);           // freed??
    return;
}

void Avatar::RecvStateInfo()
{
    if (group == NULL)
        return;

    char buf[2048];

    while(group->Recv(id, buf, 2048) != 0)
    {
        struct State_Info *stif = (struct State_Info *)buf;

        int better = agent->MergeStateInfo(stif);
        if (better == 0)                // send out my information if it is not better
        {
            struct State_Info *si = agent->GetStateInfo(stif->st);
            if (si != NULL)
            {
                group->Send(id, si, si->length);
                free(si);               // freed?
            }
        }
    }
    return;
}

