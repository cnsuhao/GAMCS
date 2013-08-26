/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "SimGroup.h"

void SimGroup::Include(Entity *ent)
{
    ent->SetSendFunc(this->Send);
    ent->SetRecvFunc(this->Recv);

    entities.push_back(ent);
    member_num++;
    return;
}

void SimGroup::Run()
{
    BuildNeighs();

    for (vector<Entity *>::iterator pentity=entities.begin(); pentity!=entities.end(); ++pentity)
    {
        pthread_t tid = *pentity->ThreadRun();
        tids.push_back(tid);
    }

    for (vector<pthread_t>::iterator it=tids.begin(); it!=tids.end(); ++it)
    {
        pthread_join(*it, NULL);
    }

    return;
}

int SimGroup::Send(int id, void *buffer, size_t length)
{
    printf("*************************** Id: %d, Send:********************************\n", id);
    struct State_Info *si = (struct State_Info *)buffer;
    MyAgent::PrintStateInfo(si);
    printf("---------------------------- Send -----------------------------\n\n");

    if (length > 2048)
    {
        printf("data length exceeds 2048!\n");
        return 0;
    }
    vector<int> neighs = GetNeighs(id);
    for (vector<int>::iterator it = neighs.begin();
    it != neighs.end(); ++it)
    {
        struct Channel *chan = GetChannel(*it);
        pthread_mutex_lock(&chan->mutex);

        struct Frame *fr = (struct Frame *)malloc(sizeof(struct Frame));    // buggy!
        memcpy(fr->data, buffer, length);
        fr->next = chan->frame;
        chan->frame = fr;

        pthread_mutex_unlock(&chan->mutex);

    }
    return length;
}

int SimGroup::Recv(int id, void *buffer, size_t length)
{

    struct Channel *chan = GetChannel(id);
    size_t re;
    pthread_mutex_lock(&chan->mutex);

    if (chan->frame != NULL)
    {
        memcpy(buffer, chan->frame->data, length);
        struct Frame *tmp = chan->frame;
        chan->frame = chan->frame->next;
        free(tmp);
        re = length;

        printf("++++++++++++++++++++++++ Id: %d, Recv: ++++++++++++++++++++++++\n", id);
        struct State_Info *si = (struct State_Info *)buffer;
        MyAgent::PrintStateInfo(si);
        printf("|||||||||||||||||||||||||||||| Recv ||||||||||||||||||||||||||||||||||\n\n");
    }
    else
        re = 0;

    pthread_mutex_unlock(&chan->mutex);
    return re;

}

void SimGroup::SetTopo(char *tf)
{
    topofile = tf;
    return;
}

SimGroup::SimGroup(int i)
{
    //ctor
    id = i;
    topofile = NULL;
    member_num = 0;
    entities.clear();
    tids.clear();
}

SimGroup::~SimGroup()
{
    //dtor
}

struct Channel *SimGroup::GetChannel(int id)
{
    return &channels[id];
};

vector<int> SimGroup::GetNeighs(int id)
{
    vector<int> neighs;
    neighs.clear();

    struct Neigh *nh, *nnh;
    for (nh=neighlist[id]; nh!=NULL; nh=nnh)
    {
        neighs.push_back(nh->id);
        nnh = nh->next;
    }
    return neighs;
}

void SimGroup::BuildNeighs()
{
    int num = 0;
    if (topofile == NULL)
    {
        dbgprt("topofile is NULL!\n");
        return;
    }

    fstream tf(topofile);

    if (!tf.is_open())
    {
        dbgprt("can't open topofile!\n");
        return;
    }

    char line[1024];
    const char *delim = ": ";
    while (!tf.eof())
    {
        tf.getline(line, 1024);
        char *p = strtok(line, delim);
        if (!p)
            break;
        int index = atoi(p);
        while(p)
        {
            p = strtok(NULL, delim);
            if (p && atoi(p) != index)          // exclude self
            {
                struct Neigh *nneigh = (struct Neigh *)malloc(sizeof(struct Neigh));
                nneigh->id = atoi(p);
                nneigh->next = neighlist[index];
                neighlist[index] = nneigh;
            }
        }
        num++;
    }
    member_num = num;
    return;
}
