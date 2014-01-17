/**********************************************************************
*	@File:
*	@Created: 2013-8-21
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include "SimGroup.h"

/**
* \brief Send message to all neighbours of a member.
* \param id the message sender id
* \param buffer buffer where the message is stored
* \param length length of the message
* \return length of message that has been sent
*/
int SimGroup::Send(int id, void *buffer, size_t length)
{
    size_t re = 0;
    if (length > DATA_SIZE) // check length
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

        vector<int> neighs = GetNeighs(id);     // get all its neightbours, the message will send to all of they
        // walk through all its neighbours, send message to they one by one
        for (vector<int>::iterator it = neighs.begin();
        it != neighs.end(); ++it)
        {
            // for a neighbour
            struct Channel *chan = GetChannel(*it); // get its channel
            pthread_mutex_lock(&chan->mutex);   // lock before write message to it

            if (chan->ptr == 0) chan->ptr = CHANNEL_SIZE - 1;   // move point, make room for the new msg, the oldest message will be lost
            else chan->ptr -= 1;
            memcpy(chan->msg[chan->ptr].data, buffer, length);  // copy message to the channel
            chan->msg[chan->ptr].sender_id = id;

            if (chan->msg_num < CHANNEL_SIZE) chan->msg_num++;  // maximum num is CHANNEL_SIZE

            pthread_mutex_unlock(&chan->mutex); // unlock
            re = length;
        }
    }
    return re;
}

/**
* \brief Recieve one message of a specified member.
* \param id member id who want to recieve
* \param buffer buffer where to store the recieved message
* \param length of the message to be recieved
* \return length of message recieved
*/
int SimGroup::Recv(int id, void *buffer, size_t length)
{
    if (length > DATA_SIZE)     // check length
    {
        dbgprt("Recv(): requested data length exceeds %d!\n", DATA_SIZE);
        return 0;
    }

    struct Channel *chan = GetChannel(id);      // get my channel
    size_t re;
    pthread_mutex_lock(&chan->mutex);       // lock it before reading, prevent concurrent writtings

    if (chan->msg_num == 0)     // no new messages
    {
        re = 0;
    }
    else
    {
        memcpy(buffer, chan->msg[chan->ptr].data, length);      // copy message to buffer
        int sid = chan->msg[chan->ptr].sender_id;       // get sender's id
        UNUSED(sid);        // FIXME: we didn't use it.
        chan->msg_num--;    // dec the message number
        if (chan->ptr == CHANNEL_SIZE -1) chan->ptr = 0;    // move the message point
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

    pthread_mutex_unlock(&chan->mutex);     // unlock
    return re;
}

/**
* \brief Load topological structure of a group from a configure file.
* \param tf file name
*/
void SimGroup::LoadTopo(string tf)
{
    topofile = tf;
    BuildNeighsChannels();
    return;
}

SimGroup::SimGroup() : topofile("")
{
    members.clear();
    for (int i=0; i<MAX_MEMBER; i++)    // set unused neightlist and channels to NULL
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

SimGroup::SimGroup(int i) : Group(i), topofile("")
{
    members.clear();
    for (int i=0; i<MAX_MEMBER; i++)
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

SimGroup::~SimGroup()
{
    for (vector<int>::iterator it = members.begin(); it!=members.end(); ++it)
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

        /* free messages in channels */
        free(channels[*it].msg);
    }

    members.clear();
}

/**
* \brief Get one's channel.
* \param id member id
* \return channel address
*/
struct Channel *SimGroup::GetChannel(int id)
{
    return &channels[id];
};

/**
* \brief Get one's all neighbours.
* \param id member id
* \return neighbour list
*/
vector<int> SimGroup::GetNeighs(int id)
{
    vector<int> neighs;
    neighs.clear();

    // walk through one's neighlist
    struct Neigh *nh, *nnh;
    for (nh=neighlist[id]; nh!=NULL; nh=nnh)
    {
        neighs.push_back(nh->id);   // get all its neighs
        nnh = nh->next;
    }
    return neighs;
}

/**
* \brief Build up neighlist and channels for all members in the group.
*/
void SimGroup::BuildNeighsChannels()
{
    if (topofile.empty())   // no topofile specified, the group will be emtpy, no members or neighbours
    {
        dbgprt("topofile is NULL!\n");
        return;
    }

    fstream tf(topofile.c_str());   // open topofile

    if (!tf.is_open())
    {
        ERROR("Group: %d can't open topofile: %s!\n", id, topofile.c_str());
    }

    /* parse file and build neighlist */
    char line[2048];    // buffer for reading a line in topofile
    const char *delim = ": ";   // delimiter bewteen member Id and its neighbour Ids
    while (!tf.eof())       // read till end
    {
        tf.getline(line, 2048);
        // get the member itself first
        char *p = strtok(line, delim);
        if (!p)
            break;
        int node = atoi(p);
        members.push_back(node);   // a new member, save it
        // parse its neighbours
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

    member_num = members.size();    // number of members

    /* initialize channels */
    for (vector<int>::iterator it = members.begin(); it!=members.end(); ++it)
    {
        channels[*it].msg = (struct Msg *)malloc(sizeof(struct Msg) * CHANNEL_SIZE);
        assert(channels[*it].msg != NULL);
        pthread_mutex_init(&(channels[*it].mutex), NULL);   // initialize mutex
        channels[*it].msg_num = 0;
        channels[*it].ptr = CHANNEL_SIZE - 1;
    }

    return;
}

/**
* \brief Get member number in group.
* \return number of members
*/
int SimGroup::NumOfMembers()
{
    return member_num;
}
