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
#include "CSThreadCommNet.h"
#include "Agent.h"
#include "Debug.h"

CSThreadCommNet::CSThreadCommNet()
{
    members.clear();
    for (int i = 0; i < MAX_MEMBER; i++)    // set unused neightlist and channels to NULL
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

CSThreadCommNet::CSThreadCommNet(int i) :
        CommNet(i)
{
    members.clear();
    for (int i = 0; i < MAX_MEMBER; i++)
    {
        neighlist[i] = NULL;
        channels[i].msg = NULL;
    }
}

CSThreadCommNet::~CSThreadCommNet()
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
 * \param sender_id the message sender id
 * \param buffer buffer where the message is stored
 * \param length length of the message
 * \return length of message that has been sent
 */
int CSThreadCommNet::Send(int sender_id, void *buffer, size_t buf_size)
{
    size_t re = 0;
    if (buf_size > DATA_SIZE)    // check size
    {
        dbgprt("Send()", "data size exceeds %d, not send!\n", DATA_SIZE);
        re = 0;
    }
    else
    {
#ifdef _DEBUG_MORE_
        printf(
                "*************************** Id: %d, Send ********************************\n",
                sender_id);
        Agent::PrintStateInfo((struct State_Info_Header *) buffer);
        printf(
                "****************************** Send End **********************************\n\n");
#endif // _DEBUG_

        std::vector<int> neighs = GetNeighbours(sender_id);    // get all its neightbours, the message will send to all of they
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
            memcpy(chan->msg[chan->ptr].data, buffer, buf_size);    // copy message to the channel
            chan->msg[chan->ptr].sender_id = sender_id;

            if (chan->msg_num < CHANNEL_SIZE) chan->msg_num++;    // maximum num is CHANNEL_SIZE

            pthread_mutex_unlock(&chan->mutex);    // unlock
            re = buf_size;
        }
    }
    return re;
}

/**
 * \brief Recieve one message of a specified member.
 * \param recver_id member id who want to recieve
 * \param buffer buffer where to store the recieved message
 * \param length of the message to be recieved
 * \return length of message recieved
 */
int CSThreadCommNet::Recv(int recver_id, void *buffer, size_t buf_size)
{
    if (buf_size > DATA_SIZE)    // check length
    {
        WARNNING("Recv()- requested data length exceeds DATA_SIZE.\n");
    }

    struct Channel *chan = GetChannel(recver_id);    // get my channel
    size_t re = 0;
    pthread_mutex_lock(&chan->mutex);    // lock it before reading, prevent concurrent writtings

    if (chan->msg_num == 0)    // no new messages
    {
        re = 0;
    }
    else
    {
        memcpy(buffer, chan->msg[chan->ptr].data, buf_size);    // copy message to buffer
        int sid = chan->msg[chan->ptr].sender_id;    // get sender's id
        UNUSED(sid);    // FIXME: we didn't use it.
        chan->msg_num--;    // dec the message number
        if (chan->ptr == CHANNEL_SIZE - 1)
            chan->ptr = 0;    // move the message point
        else
            chan->ptr += 1;

        struct State_Info_Header *stif = (struct State_Info_Header *) buffer;
        if (stif->size > buf_size)    // check size
        {
            WARNNING(
                    "Recv()- requested state information exceeds buffer size!.\n");
            re = 0;    // don't return incompelte information
        }
        else
            re = buf_size;    // ok, msg is recieved
#ifdef _DEBUG_MORE_
        printf(
                "++++++++++++++++++++++++ Id: %d, Recv from: %d ++++++++++++++++++++++++\n",
                recver_id, sid);
        Agent::PrintStateInfo(stif);
        printf(
                "++++++++++++++++++++++++++++++ Recv End ++++++++++++++++++++++++++++++\n\n");
#endif // _DEBUG_
    }

    pthread_mutex_unlock(&chan->mutex);    // unlock
    return re;
}

/**
 * \brief Load topological structure of a group from a configure file.
 * \param tf file name
 */
void CSThreadCommNet::LoadTopoFile(std::string tf)
{
    if (tf.empty())    // no topofile specified, the group will be emtpy, no members or neighbours
    {
        dbgprt("INFO", "topofile is NULL!\n");
        return;
    }

    std::fstream topofs(tf.c_str());    // open topofile

    if (!topofs.is_open())
    {
        ERROR("Group: %d can't open topofile: %s!\n", id, tf.c_str());
    }

    /* parse file, add member and  build neighlist */
    char line[4096];    // buffer for reading a line in topofile, make sure it's big enough
    const char *delim = ": ";    // delimiter bewteen member Id and its neighbour Ids
    while (!topofs.eof())    // read till end
    {
        topofs.getline(line, 4096);
        // get the member itself first
        char *p = strtok(line, delim);
        if (!p) break;

        int mid = atoi(p);
        AddMember(mid);    // a new member, add it

        // parse its neighbours
        while (p)
        {
            p = strtok(NULL, delim);
            if (p && (atoi(p) != mid))    // exclude self
            {
                int nid = atoi(p);      // neighbour id
                AddNeighbour(mid, nid); // add nid as a neighbour of mid
            }
        }
    }

    return;
}

/**
 * \brief Get one's channel.
 * \param id member id
 * \return channel address
 */
struct Channel *CSThreadCommNet::GetChannel(int id)
{
    return &channels[id];
}
;

/**
 * \brief Get one's all neighbours.
 * \param id member id
 * \return neighbour list
 */
std::vector<int> CSThreadCommNet::GetNeighbours(int id)
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

void CSThreadCommNet::AddMember(int mem)
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

void CSThreadCommNet::AddNeighbour(int mem, int neb)
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

void CSThreadCommNet::RemoveMember(int mem)
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
    neighlist[mem] = NULL;    //don't forget to set to NULL

    /* destroy mutex */
    pthread_mutex_destroy(&(channels[mem].mutex));

    /* free messages in channels */
    free(channels[mem].msg);

    // remove member
    for (std::vector<int>::iterator mit = members.begin(); mit != members.end();
            )
    {
        if ((*mit) == mem)    // found
        {
            members.erase(mit);
            break;
        }
        else
            ++mit;
    }

}

void CSThreadCommNet::RemoveNeighbour(int mem, int neighbour)
{
    // check if member exists
    std::vector<int>::iterator it = std::find(members.begin(), members.end(),
            mem);
    if (it == members.end())    // not found
    {
        dbgprt("Error", "Member %d not exists, can not remove its neighbour\n",
                mem);
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

bool CSThreadCommNet::CheckNeighbourShip(int from, int to)
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

bool CSThreadCommNet::HasMember(int id)
{
    bool re = false;
    std::vector<int>::iterator it = std::find(members.begin(), members.end(), id);
    if (it != members.end())    // found
        re = true;

    return re;
}
