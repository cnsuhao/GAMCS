/**********************************************************************
 *	@File:
 *	@Created: 2013-8-21
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <climits>
#include <graphviz/cgraph.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
    for (std::set<int>::iterator it = members.begin(); it != members.end();
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
 * \brief Send message to a neighbour
 * \param sender_id the message sender id
 * \param buffer buffer where the message is stored
 * \param length length of the message
 * \return length of message that has been sent to the neighbour
 */
int CSThreadCommNet::Send(int fromid, int toid, void *buffer, size_t buf_size)
{
    if (buf_size > DATA_SIZE)    // check size
    {
        dbgprt("Send()", "data size exceeds %d, not send!\n", DATA_SIZE);
        return 0;
    }

#ifdef _DEBUG_MORE_
    printf(
            "*************************** %d send msg to %d ********************************\n",
            fromid, toid);
    Agent::PrintStateInfo((struct State_Info_Header *) buffer);
    printf(
            "****************************** Send End **********************************\n\n");
#endif // _DEBUG_

    struct Channel *chan = GetChannel(toid);    // get its channel
    pthread_mutex_lock(&chan->mutex);    // lock before write message to it

    if (chan->ptr == 0)
        chan->ptr = CHANNEL_SIZE - 1;    // move point, make room for the new msg, the oldest message will be lost
    else
        chan->ptr -= 1;

    // check if msg buffer is not NULL, this may happen when a member has a neighbour that has been removed from net.
    if (chan->msg == NULL)    // neighbour has been removed from network
    {
        pthread_mutex_unlock(&chan->mutex);    // unlock
        RemoveNeighbour(fromid, toid);    // break up with removed member
        return 0;    // no msg is sent
    }

    memcpy(chan->msg[chan->ptr].data, buffer, buf_size);    // copy message to the channel
    chan->msg[chan->ptr].sender_id = fromid;

    if (chan->msg_num < CHANNEL_SIZE) chan->msg_num++;    // maximum num is CHANNEL_SIZE

    pthread_mutex_unlock(&chan->mutex);    // unlock

    return buf_size;
}

/**
 * \brief Recieve one message of a specified member.
 * \param recver_id member id who want to recieve
 * \param buffer buffer where to store the recieved message
 * \param length of the message to be recieved
 * \return length of message recieved
 */
int CSThreadCommNet::Recv(int toid, int fromid, void *buffer, size_t buf_size)
{
    if (buf_size > DATA_SIZE)    // check length
    {
        WARNNING("Recv()- requested data length exceeds DATA_SIZE.\n");
    }

    struct Channel *chan = GetChannel(toid);    // get my channel
    size_t re = 0;
    pthread_mutex_lock(&chan->mutex);    // lock it before reading, prevent concurrent writtings

    if (chan->msg_num == 0)    // no new messages
    {
        re = 0;
    }
    else
    {
        if (fromid == -1)    // recieve msg from any agent
        {
            memcpy(buffer, chan->msg[chan->ptr].data, buf_size);    // copy a message to buffer
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
                    "++++++++++++++++++++++++ %d recv msg from %d ++++++++++++++++++++++++\n",
                    toid, sid);
            Agent::PrintStateInfo(stif);
            printf(
                    "++++++++++++++++++++++++++++++ Recv End ++++++++++++++++++++++++++++++\n\n");
#endif // _DEBUG_
        }
        else
            ERROR(
                    "Recv(): This function doesn't support recieve message from a specified agent currently!\n");    // TODO
    }

    pthread_mutex_unlock(&chan->mutex);    // unlock
    return re;
}

/**
 * \brief Get one's all neighbours.
 * \param id member id
 * \return neighbour list
 */
std::set<int> CSThreadCommNet::GetNeighbours(int id)
{
    std::set<int> neighs;
    neighs.clear();

    // walk through one's neighlist
    struct Neigh *nh, *nnh;
    for (nh = neighlist[id]; nh != NULL; nh = nnh)
    {
        neighs.insert(nh->id);    // get all its neighs
        nnh = nh->next;
    }
    return neighs;
}

void CSThreadCommNet::AddMember(int mem)
{
    // chech if alread exists
    if (members.find(mem) != members.end())    // found
    return;

    members.insert(mem);
    /* initialize member's channel */
    channels[mem].msg = (struct Msg *) malloc(    // allocate buffer for msgs
            sizeof(struct Msg) * CHANNEL_SIZE);
    assert(channels[mem].msg != NULL);
    pthread_mutex_init(&(channels[mem].mutex), NULL);
    channels[mem].msg_num = 0;
    channels[mem].ptr = CHANNEL_SIZE - 1;
}

void CSThreadCommNet::AddNeighbour(int mem, int neb, int interval)
{
    // check if member exists
    if (members.find(mem) == members.end())    // not found
    {
        WARNNING("AddNeighbour(): Member %d not exists, add it first\n", mem);
        return;
    }

    // check if neighbour exists
    if (members.find(neb) == members.end())    // not found
    {
        WARNNING(
                "AddNeighbour(): Neighbour %d not exists, can not make neighbour with it!\n",
                mem);
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
        WARNNING("AddNeighbour(): member %d already has neighbour %d.\n", mem,
                neb);
        return;
    }

    // add the neighbour
    struct Neigh *nneigh = (struct Neigh *) malloc(sizeof(struct Neigh));
    assert(nneigh != NULL);
    nneigh->id = neb;
    nneigh->interval = interval;
    nneigh->next = neighlist[mem];
    neighlist[mem] = nneigh;
}

int CSThreadCommNet::GetNeighCommInterval(int mem, int neb)
{
    struct Neigh *nb, *nnb;
    for (nb = neighlist[mem]; nb != NULL; nb = nnb)
    {
        if (nb->id == neb)    // neighbour found
        {
            return nb->interval;
        }

        nnb = nb->next;
    }

    if (nb == NULL)    // neighbour not exists
    {
        WARNNING("GetNeighCommInterval(): member %d doesn't have neighbour %d\n", mem,
                neb);
    }
    return INT_MAX;    // return a maximum possible interval
}

void CSThreadCommNet::ChangeNeighCommInterval(int mem, int neb, int newinterval)
{
    struct Neigh *nb, *nnb;
    for (nb = neighlist[mem]; nb != NULL; nb = nnb)
    {
        if (nb->id == neb)    // neighbour found
        {
            nb->interval = newinterval;    // set new interval
            return;
        }

        nnb = nb->next;
    }

    if (nb == NULL)    // neighbour not exists
    {
        WARNNING("ChangeNeighCommInterval(): member %d doesn't have neighbour %d\n",
                mem, neb);
    }
    return;    // return a maximum possible interval
}

void CSThreadCommNet::RemoveMember(int mem)
{
    // check if exists
    if (members.find(mem) == members.end())    // not found
    {
        WARNNING("RemoveMember(): member %d not exists, can not remove\n", mem);
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
    channels[mem].msg = NULL;    //set to NULL

    // remove member
    members.erase(mem);
}

void CSThreadCommNet::RemoveNeighbour(int mem, int neighbour)
{
    // check if member exists
    if (members.find(mem) == members.end())    // not found
    {
        WARNNING(
                "RemoveNeighbour(): Member %d not exists, can not remove its neighbour\n",
                mem);
        return;
    }

    // check if neighbour exists
    if (members.find(neighbour) == members.end())    // not found
    {
        WARNNING("RemoveNeighbour(): Neighbour %d not exists in network!\n",
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

/**
 * \brief Load topological structure of a group from a configure file.
 * This function will not add new members but just make neighbours.
 * \param tf file name
 */
void CSThreadCommNet::LoadTopoFromFile(char *tf)
{
    Agraph_t *graph;
    Agnode_t *node, *neigh_node;
    Agedge_t *edge;

    FILE *topofs = fopen(tf, "r");
    if (topofs == NULL)
    ERROR("LoadTopoFromFile: network %d can't open topofile: %s!\n", id, tf);

    // load graph from file
    graph = agread(topofs, 0);
    fclose(topofs);

    // traversal each node in graph
    for (node = agfstnode(graph); node; node = agnxtnode(graph, node))
    {
        int mid = atoi(agget(node, "id"));    // get agent id
        // check if member has joined in network
        if (!HasMember(mid))    // not join
        {
            WARNNING(
                    "LoadTopoFromFile: can not add neighbours for member %d, it's not in network, join in first!\n",
                    mid);
            continue;
        }

        // get its neighbours
        for (edge = agfstout(graph, node); edge; edge = agnxtout(graph, edge))
        {
            int interval = atoi(agget(edge, "interval"));    // get interval from edge
            neigh_node = edge->node;
            int neb = atoi(agget(neigh_node, "id"));    // get neighbour's id

            AddNeighbour(mid, neb, interval);
        }

    }
    agclose(graph);
    return;
}

/**
 * \brief Dump structure of communication network to file
 */
void CSThreadCommNet::DumpTopoToFile(char *tf)
{
    FILE *topofs = fopen(tf, "w+");
    if (topofs == NULL)
    ERROR("DumpTopoToFile: network %d can't open topofile: %s!\n", id, tf);

    /* topo file example:
     * diagraph CommNet_1 {
     * label = "#members: 3, ..."
     * node [color=black, shape=circle]
     * rank="same"
     *
     * mem1 [label="1", id="1"]
     * mem2 [label="2", id="2"]
     * mem3 [label="3", id="3"]
     *
     * mem1 -> mem2 [label="100", interval="100"]
     * mem3 -> mem1 [label="50" , interval="50"]
     * }
     */

    fprintf(topofs, "/* Topological structure dumped by CommNet %d */\n\n", id);
    fprintf(topofs, "digraph CommNet_%d \n{\n", id);
    fprintf(topofs,
            "label=\"Topo Structure of CommNet %d\\n#members: %d, ...\"\n", id,
            NumberOfMembers());     // statistics about the network can be put here
    fprintf(topofs, "node [color=\"black\", shape=\"circle\"]\n");
    fprintf(topofs, "rank=\"same\"\n");

    std::set<int> allmembers = GetAllMembers();
    for (std::set<int>::iterator mit = allmembers.begin();
            mit != allmembers.end(); ++mit)
    {
        fprintf(topofs, "\nmem%d [label=\"%d\", id=\"%d\"]\n", *mit, *mit,
                *mit);
        std::set<int> neighbours = GetNeighbours(*mit);
        for (std::set<int>::iterator nit = neighbours.begin();
                nit != neighbours.end(); ++nit)
        {
            int interval = GetNeighCommInterval(*mit, *nit);    // get interval
            fprintf(topofs, "mem%d -> mem%d [label=\"%d\", interval=\"%d\"]\n",
                    *mit, *nit, interval, interval);
        }
    }

    fprintf(topofs, "}\n");    // diagraph
    fclose(topofs);
}
