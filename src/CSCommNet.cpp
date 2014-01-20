/**********************************************************************
 *	@File:
 *	@Created: 2013-8-21
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "CSCommNet.h"
#include "CSAgent.h"
#include "Debug.h"

CSCommNet::CSCommNet() :
        topofile(""), member_num(0)
{
    members.clear();
    for (int i = 0; i < MAX_MEMBER; i++)    // set unused neightlist and channels to NULL
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

CSCommNet::CSCommNet(int i) :
        CommNet(i), topofile(""), member_num(0)
{
    members.clear();
    for (int i = 0; i < MAX_MEMBER; i++)
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

CSCommNet::~CSCommNet()
{
    for (std::vector<int>::iterator it = members.begin(); it != members.end();
            ++it)
    {
        /* free neighlist */
        struct Neigh *neigh = NULL, *nneigh = NULL;
        for (neigh = neighlist[*it]; neigh != NULL; neigh = nneigh)
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
 * \brief Send message to all neighbours of a member.
 * \param id the message sender id
 * \param buffer buffer where the message is stored
 * \param length length of the message
 * \return length of message that has been sent
 */
int CSCommNet::Send(int id, const void *buffer, size_t length)
{
    size_t re = 0;
    if (length > DATA_SIZE)    // check length
    {
        dbgprt("Send()", "data length exceeds %d, not send!\n", DATA_SIZE);
        re = 0;
    }
    else
    {
        struct State_Info_Header *si = (struct State_Info_Header *)buffer;
#ifdef _DEBUG_
        printf("*************************** Id: %d, Send ********************************\n", id);
        CSAgent::PrintStateInfo(si);
        printf("------------------------------ Send End----------------------------------\n\n");
#endif // _DEBUG_

        std::vector<int> neighs = GetNeighbours(id);    // get all its neightbours, the message will send to all of they
        // walk through all its neighbours, send message to they one by one
        for (std::vector<int>::iterator it = neighs.begin(); it != neighs.end();
                ++it)
        {
            // for a neighbour
            struct Channel *chan = GetChannel(*it);    // get its channel
            pthread_mutex_lock(&chan->mutex);    // lock before write message to it

            if (chan->ptr == 0)
                chan->ptr = CHANNEL_SIZE - 1;    // move point, make room for the new msg, the oldest message will be lost
            else
                chan->ptr -= 1;
            memcpy(chan->msg[chan->ptr].data, buffer, length);    // copy message to the channel
            chan->msg[chan->ptr].sender_id = id;

            if (chan->msg_num < CHANNEL_SIZE) chan->msg_num++;    // maximum num is CHANNEL_SIZE

            pthread_mutex_unlock(&chan->mutex);    // unlock
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
int CSCommNet::Recv(int id, void *buffer, size_t length)
{
    if (length > DATA_SIZE)    // check length
    {
        WARNNING("Recv()- requested data length exceeds %d!\n", DATA_SIZE);
        return 0;
    }

    struct Channel *chan = GetChannel(id);    // get my channel
    size_t re;
    pthread_mutex_lock(&chan->mutex);    // lock it before reading, prevent concurrent writtings

    if (chan->msg_num == 0)    // no new messages
    {
        re = 0;
    }
    else
    {
        memcpy(buffer, chan->msg[chan->ptr].data, length);    // copy message to buffer
        int sid = chan->msg[chan->ptr].sender_id;    // get sender's id
        UNUSED(sid);    // FIXME: we didn't use it.
        chan->msg_num--;    // dec the message number
        if (chan->ptr == CHANNEL_SIZE - 1)
            chan->ptr = 0;    // move the message point
        else
            chan->ptr += 1;

        re = length;
        struct State_Info_Header *si = (struct State_Info_Header *)buffer;
#ifdef _DEBUG_
        printf("++++++++++++++++++++++++ Id: %d, Recv from: %d ++++++++++++++++++++++++\n", id, sid);
        CSAgent::PrintStateInfo(si);
        printf("|||||||||||||||||||||||||||||| Recv End |||||| |||||||||||||||||||||||\n\n");
#endif // _DEBUG_
    }

    pthread_mutex_unlock(&chan->mutex);    // unlock
    return re;
}

/**
 * \brief Load topological structure of a group from a configure file.
 * \param tf file name
 */
void CSCommNet::LoadTopoFile(std::string tf)
{
    topofile = tf;
    BuildNeighsChannels();
    return;
}

/**
 * \brief Get one's channel.
 * \param id member id
 * \return channel address
 */
struct Channel *CSCommNet::GetChannel(int id)
{
    return &channels[id];
}
;

/**
 * \brief Get one's all neighbours.
 * \param id member id
 * \return neighbour list
 */
std::vector<int> CSCommNet::GetNeighbours(int id)
{
    std::vector<int> neighs;
    neighs.clear();

    // walk through one's neighlist
    struct Neigh *nh, *nnh;
    for (nh = neighlist[id]; nh != NULL; nh = nnh)
    {
        neighs.push_back(nh->id);    // get all its neighs
        nnh = nh->next;
    }
    return neighs;
}

/**
 * \brief Build up neighlist and channels for all members in the group.
 */
void CSCommNet::BuildNeighsChannels()
{
    if (topofile.empty())    // no topofile specified, the group will be emtpy, no members or neighbours
    {
        dbgprt("INFO", "topofile is NULL!\n");
        return;
    }

    std::fstream tf(topofile.c_str());    // open topofile

    if (!tf.is_open())
    {
        ERROR("Group: %d can't open topofile: %s!\n", id, topofile.c_str());
    }

    /* parse file and build neighlist */
    char line[2048];    // buffer for reading a line in topofile
    const char *delim = ": ";    // delimiter bewteen member Id and its neighbour Ids
    while (!tf.eof())    // read till end
    {
        tf.getline(line, 2048);
        // get the member itself first
        char *p = strtok(line, delim);
        if (!p) break;
        int node = atoi(p);
        members.push_back(node);    // a new member, save it
        // parse its neighbours
        while (p)
        {
            p = strtok(NULL, delim);
            if (p && atoi(p) != node)    // exclude self
            {
                struct Neigh *nneigh = (struct Neigh *) malloc(
                        sizeof(struct Neigh));
                assert(nneigh != NULL);
                nneigh->id = atoi(p);
                nneigh->next = neighlist[node];
                neighlist[node] = nneigh;
            }
        }
    }

    member_num = members.size();    // number of members

    /* initialize channels */
    for (std::vector<int>::iterator it = members.begin(); it != members.end();
            ++it)
    {
        channels[*it].msg = (struct Msg *) malloc(
                sizeof(struct Msg) * CHANNEL_SIZE);
        assert(channels[*it].msg != NULL);
        pthread_mutex_init(&(channels[*it].mutex), NULL);    // initialize mutex
        channels[*it].msg_num = 0;
        channels[*it].ptr = CHANNEL_SIZE - 1;
    }

    return;
}


void CSCommNet::AddMember(int mid, const std::vector<int> &neighbours)
{
    AddMember(mid); // add member first
    AddNeighbour(mid, neighbours);  // add neighbours
}

void CSCommNet::AddNeighbour(int who, const std::vector<int> &neighbours)
{
    // add all neighbours
    for (std::vector<int>::const_iterator nit = neighbours.begin();
            nit != neighbours.end(); ++nit)
    {
        AddNeighbour(who, *nit);
    }
}

void CSCommNet::RemoveNeighbour(int who, const std::vector<int> &neighbours)
{
    // remove all neighbours
    for (std::vector<int>::const_iterator nit = neighbours.begin();
            nit != neighbours.end(); ++nit)
    {
        RemoveNeighbour(who, *nit);
    }
}

void CSCommNet::AddMember(int mem)
{
    // chech if alread exists
    std::vector<int>::iterator it = std::find(members.begin(), members.end(),
            mem);
    if (it != members.end())    // found
    return;

    members.push_back(mem);
    /* initialize member's channel */
    channels[mem].msg = (struct Msg *) malloc(
            sizeof(struct Msg) * CHANNEL_SIZE);
    assert(channels[mem].msg != NULL);
    pthread_mutex_init(&(channels[mem].mutex), NULL);
    channels[mem].msg_num = 0;
    channels[mem].ptr = CHANNEL_SIZE - 1;
}

void CSCommNet::AddNeighbour(int mem, int neb)
{
    // check if member exists
    std::vector<int>::iterator it = std::find(members.begin(), members.end(),
            mem);
    if (it == members.end())    // not found
    {
        dbgprt("Error", "Member %d not exists, add it first\n", mem);
        return;
    }

    // check if the neighbour already exists
    bool neigh_exist = false;
    struct Neigh *nb, *nnb;
    for (nb = neighlist[mem]; nb != NULL; nb = nnb)
    {
        if (nb->id == neb)
        {
            neigh_exist = true;
            break;
        }
        else
        {
            nnb = nb->next;
        }
    }

    if (neigh_exist == true)    // neighbour already exists, not add again
    {
        dbgprt("Warnning", "member %d already has neighbour %d.", mem, neb);
        return;
    }

    // add the neighbour
    struct Neigh *nneigh = (struct Neigh *) malloc(sizeof(struct Neigh));
    assert(nneigh != NULL);
    nneigh->id = neb;
    nneigh->next = neighlist[mem];
    neighlist[mem] = nneigh;
}

void CSCommNet::RemoveMember(int mem)
{
    // check if exists
    std::vector<int>::iterator it = std::find(members.begin(), members.end(),
            mem);
    if (it == members.end())    // not found
    {
        dbgprt("Warnning", "member %d not exists, can not remove\n", mem);
        return;
    }

    /* free neighlist */
    struct Neigh *neigh = NULL, *nneigh = NULL;
    for (neigh = neighlist[mem]; neigh != NULL; neigh = nneigh)
    {
        nneigh = neigh->next;
        free(neigh);
    }
    neighlist[mem] = NULL;      //don't forget to set to NULL

    /* destroy mutex */
    pthread_mutex_destroy(&(channels[mem].mutex));

    /* free messages in channels */
    free(channels[mem].msg);

    // remove member
    for (std::vector<int>::iterator mit = members.begin(); mit!=members.end();)
    {
        if ((*mit) == mem)  // found
        {
            members.erase(mit);
            break;
        }
        else
            ++mit;
    }

}

void CSCommNet::RemoveNeighbour(int mem, int neighbour)
{
    // check if member exists
    std::vector<int>::iterator it = std::find(members.begin(), members.end(),
            mem);
    if (it == members.end())    // not found
    {
        dbgprt("Error", "Member %d not exists, can not remove its neighbour\n", mem);
        return;
    }

    // find the neighbour and remove it
    struct Neigh *nb, *pnb = NULL;
    for (nb = neighlist[mem]; nb != NULL; pnb = nb, nb = pnb->next)
    {
        if (nb->id == neighbour)    // Found it
        {
            if (pnb == NULL)
            {
                // fix beginning pointer
                neighlist[mem] = nb->next;
            }
            else
            {
                // fix previous node's next to skip over the removed node
                pnb->next = nb->next;
            }

            // deallocate the neighbour
            free(nb);

            return;
        }
    }

}

bool CSCommNet::IsConnectedTo(int from, int to)
{
    bool connected = false;
    struct Neigh *nb, *nnb;
    for (nb = neighlist[from]; nb != NULL; nb = nnb)
    {
        if (nb->id == to)    // found
        {
            connected = true;
            break;
        }
        else
            nnb = nb->next;
    }
    return connected;
}
