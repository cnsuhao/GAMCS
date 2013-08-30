/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "SimGroup.h"

int SimGroup::Send(int id, void *buffer, size_t length)
{
    size_t re;
    if (length > DATA_SIZE)
    {
        dbgprt("Send(): data length exceeds %d, not send!\n", DATA_SIZE);
        re = 0;
    }
    else
    {
        #ifdef _DEBUG_
        printf("*************************** Id: %d, Send ********************************\n", id);
        struct State_Info_Header *si = (struct State_Info_Header *)buffer;
        #ifdef _DEBUG_
        MyAgent::PrintStateInfo(si);
        #endif // _DEBUG_
        printf("------------------------------ Send End----------------------------------\n\n");
        #endif
        vector<int> neighs = GetNeighs(id);
        for (vector<int>::iterator it = neighs.begin();
        it != neighs.end(); ++it)
        {
            struct Channel *chan = GetChannel(*it);
            pthread_mutex_lock(&chan->mutex);

            if (chan->ptr == 0) chan->ptr = CHANNEL_SIZE - 1;   // make room for new msg
            else chan->ptr -= 1;
            memcpy(chan->msg[chan->ptr].data, buffer, length);
            chan->msg[chan->ptr].sender_id = id;

            if (chan->msg_num < CHANNEL_SIZE) chan->msg_num++;  // maximum num is CHANNEL_SIZE

            pthread_mutex_unlock(&chan->mutex);
            re = length;
        }
    }
    return re;
}

int SimGroup::Recv(int id, void *buffer, size_t length)
{
    if (length > DATA_SIZE)
    {
        dbgprt("Recv(): requested data length exceeds %d!\n", DATA_SIZE);
        return 0;
    }

    struct Channel *chan = GetChannel(id);
    size_t re;
    pthread_mutex_lock(&chan->mutex);

    if (chan->msg_num == 0)
    {
        re = 0;
    }
    else
    {
        memcpy(buffer, chan->msg[chan->ptr].data, length);
        int sid = chan->msg[chan->ptr].sender_id;
        UNUSED(sid);
        chan->msg_num--;
        if (chan->ptr == CHANNEL_SIZE -1) chan->ptr = 0;
        else chan->ptr += 1;

        re = length;
#ifdef _DEBUG_
        printf("++++++++++++++++++++++++ Id: %d, Recv from: %d ++++++++++++++++++++++++\n", id, sid);
        struct State_Info_Header *si = (struct State_Info_Header *)buffer;
        #ifdef _DEBUG_
        MyAgent::PrintStateInfo(si);
        #endif // _DEBUG_
        printf("|||||||||||||||||||||||||||||| Recv End |||||| |||||||||||||||||||||||\n\n");
#endif
    }

    pthread_mutex_unlock(&chan->mutex);
    return re;
}

void SimGroup::LoadTopo(string tf)
{
    topofile = tf;
    BuildNeighsChannels();
    return;
}

SimGroup::SimGroup(int i)
{
    //ctor
    id = i;
    topofile = "";
    member_num = 0;
    nodes.clear();
    for (int i=0; i<MAX_MEMBER; i++)
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

SimGroup::~SimGroup()
{
    //dtor
    for (vector<int>::iterator it = nodes.begin(); it!=nodes.end(); ++it)
    {
        /* free neighlist */
        struct Neigh *neigh = NULL, *nneigh = NULL;
        for (neigh=neighlist[*it]; neigh!=NULL; neigh=nneigh)
        {
            nneigh = neigh->next;
            free(neigh);
        }

        /* destroy mutex */
        pthread_mutex_destroy(&(channels[*it].mutex));

        /* free Msg of channels */
        free(channels[*it].msg);
    }

    nodes.clear();
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

void SimGroup::BuildNeighsChannels()
{
    if (topofile.empty())
    {
        dbgprt("topofile is NULL!\n");
        return;
    }

    fstream tf(topofile.c_str());

    if (!tf.is_open())
    {
        ERROR("Group: %d can't open topofile: %s!\n", id, topofile.c_str());
    }

    /* parse file and build neighlist */
    char line[1024];
    const char *delim = ": ";
    while (!tf.eof())
    {
        tf.getline(line, 1024);
        char *p = strtok(line, delim);
        if (!p)
            break;
        int node = atoi(p);
        nodes.push_back(node);
        while(p)
        {
            p = strtok(NULL, delim);
            if (p && atoi(p) != node)          // exclude self
            {
                struct Neigh *nneigh = (struct Neigh *)malloc(sizeof(struct Neigh));
                assert(nneigh != NULL);
                nneigh->id = atoi(p);
                nneigh->next = neighlist[node];
                neighlist[node] = nneigh;
            }
        }
    }

    member_num = nodes.size();

    /* initialize channels */
    for (vector<int>::iterator it = nodes.begin(); it!=nodes.end(); ++it)
    {
        channels[*it].msg = (struct Msg *)malloc(sizeof(struct Msg) * CHANNEL_SIZE);
        assert(channels[*it].msg != NULL);
        pthread_mutex_init(&(channels[*it].mutex), NULL);
        channels[*it].msg_num = 0;
        channels[*it].ptr = CHANNEL_SIZE - 1;
    }

    return;
}

int SimGroup::NumOfMembers()
{
    return member_num;
}
