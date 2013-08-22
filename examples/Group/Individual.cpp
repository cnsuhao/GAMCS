/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "Individual.h"

Individual::Individual(int i, SimGroup *gp)
{
    id = i;
    printf("My id: %d\n", id);
    freq = 5;
    position = 5;
    ra = new R1Agent(15,2,0.8,0.01,"mem_file");
    grp = gp;
}

Individual::~Individual()
{
    //dtor
}

void Individual::SendStateInfo(State st)
{
    struct State_Info *si = ra->GetStateInfo(st);       // can be NULL
    if (si == NULL)
    {
        printf("si == NULL\n");
        return;
    }

    grp->Send(id, si, si->length);

    free(si);           // freed??
    return;
}


void Individual::RecvStateInfo()
{

    char buf[2048];

    while(grp->Recv(id, buf, 2048) != 0)
    {
        struct State_Info *stif = (struct State_Info *)buf;

        int better = ra->MergeStateInfo(stif);
        if (better == 0)                // send out my information if it is not better
        {
            struct State_Info *si = ra->GetStateInfo(stif->st);
            if (si != NULL)
            {
                grp->Send(id, si, si->length);
                free(si);               // freed?
            }
        }
    }
    return;
}

void Individual::SetFreq(int fq)
{
    freq = fq;
    return;
}

int Individual::ThreadRun()
{
        pthread_t tid;
        pthread_create(&tid, NULL, hook, this);

        return tid;
}

void Individual::Run()
{

    int count = 0;
    while(1)
    {
        printf("========================== Thread: %ld ==========================\n", (long)syscall(224));
        RecvStateInfo();
        State cs = GetCurrentState();
        printf("Id: %d, Current state: %ld\n", id, cs);
        Action act = ra->Process(cs);
        if (act == -1)
            break;
        DoAction(act);

        if (count >= freq)
        {
            printf("Send in Run()......\n");
            SendStateInfo(cs);
            count = 0;
        }
        else
            count++;
    }
    return;
}

State Individual::GetCurrentState()
{
    return position;
}

void Individual::DoAction(Action act)
{
    if (act == 1)
        position -= 1;
    else if (act == 2)
        position += 1;

    if (position > 15)
        position = 15;
    if (position < 1)
        position = 1;
    return;
}
