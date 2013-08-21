/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "SimGroup.h"

int SimGroup::Send(int id, void *buffer, size_t length)
{
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

        struct Frame *fr = (struct Frame *)malloc(sizeof(struct Frame *));
        memcpy(fr->data, buffer, length);
        fr->next = chan->frame;
        chan->frame = fr;

        pthread_mutex_unlock(&chan->mutex);

        Notify(*it);
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
    }
    else
        re = 0;

    pthread_mutex_unlock(&chan->mutex);
    return re;

}

SimGroup::SimGroup(string cf)
{
    //ctor
    configfile = cf;
    freq = 20;

    BuildNeighs();
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

    struct NeighInfo ni = neighlist[id];
    struct Neigh *nh, *nnh;
    for (nh=ni.neigh; nh!=NULL; nh=nnh)
    {
        neighs.push_back(nh->id);
        nnh = nh->next;
    }
    return neighs;
}

void SimGroup::Notify(int id)
{
    pthread_t tid = ThreadID[id];

    pthread_kill(tid, SIGUSR1);
    return;
}

void SimGroup::BuildNeighs()
{
    member_num;
}

void SimGroup::GroupRun()
{
    int i;
    for (i=0; i<member_num; i++)
    {
        Individual member(i)
        int ret = pthread_create((&(ThreadID[i].tid)), NULL, member.Run(), NULL);
    }
    //LOOP
}
