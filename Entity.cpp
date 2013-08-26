/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Entity.h"
#include "MyAgent.h"

Entity::Entity(int i)
{
    //ctor
    id = i;
    freq = 100;
    agent = NULL;
    Send = NULL;
    Recv = NULL;
}

Entity::~Entity()
{
    //dtor
}

int Entity::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}

void Entity::Run()
{
    int count = 0;
    while(1)
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
    return;
}

void Entity::SetFreq(int fq)
{
    freq = fq;
    return;
}

void Entity::SetAgent(MyAgent *agt)
{
    agent = agt;
    return;
}

void Entity::SetSendFunc(SEND_FUN sf)
{
    Send = sf;
    return;
}

void Entity::SetRecvFunc(RECV_FUN rf)
{
    Recv = rf;
    return;
}

void Entity::SendStateInfo(State st)
{
    if (Send == NULL)
        return;

    struct State_Info *si = agent->GetStateInfo(st);       // can be NULL
    if (si == NULL)
    {
        printf("si == NULL\n");
        return;
    }

    Send(id, si, si->length);

    free(si);           // freed??
    return;
}

void Entity::RecvStateInfo()
{
    if (Recv == NULL)
        return;

    char buf[2048];

    while(Recv(id, buf, 2048) != 0)
    {
        struct State_Info *stif = (struct State_Info *)buf;

        int better = agent->MergeStateInfo(stif);
        if (better == 0)                // send out my information if it is not better
        {
            struct State_Info *si = agent->GetStateInfo(stif->st);
            if (si != NULL)
            {
                Send(id, si, si->length);
                free(si);               // freed?
            }
        }
    }
    return;
}

