/**********************************************************************

 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdio.h>
#include "IAgent.h"
#include "DENet.h"
#include "Debug.h"

IAgent::IAgent() :
        id(0), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0), denet(
        NULL)
{
}

IAgent::IAgent(int i) :
        id(i), discount_rate(0.8), threshold(0.01), degree_of_curiosity(0.0), denet(
        NULL)
{
}

IAgent::IAgent(int i, float dr, float th) :
        id(i), discount_rate(dr), threshold(th), degree_of_curiosity(0.0), denet(
                NULL)
{
    // check validity
    if (discount_rate >= 1.0 || discount_rate < 0)    // discount rate range [0, 1)
        ERROR(
                "Agent - discount rate must be bigger than 0 and smaller than 1.0!\n");

    if (threshold < 0)
    ERROR("Agent - threshold must be bigger than 0!\n");
}

IAgent::~IAgent()
{
}

/** \brief Restrict capacity of an agent.
 *  Comply with maximun payoff rule.
 * \param st state identity
 * \param acts all possible actions of st
 * \return action distribution after appling maximun payoff restrict
 *
 */
OSpace IAgent::Restrict(IAgent::State st,
        OSpace &acts)
{
    return MaxPayoffRule(st, acts);
}

/** \brief Update inner states.
 *
 */

void IAgent::Update(float oripayoff)
{
    UpdateMemory(oripayoff);    // update memory
    TSGIOM::Update();
    return;
}

void IAgent::Exchange()
{
    if (denet == NULL)    // no ienet joined, nothing to do
        return;

    RecvStateInfo();    // check if new message has recieved

    int each_sharing_interval;
    std::set<int> my_neighbours = GetMyNeighbours();    // walk through all neighbours to check interval
    for (std::set<int>::iterator nit = my_neighbours.begin();
            nit != my_neighbours.end(); ++nit)
    {
        each_sharing_interval = GetExchangeInterval(*nit);    // get sharing interval to this neighbour
        if (process_count % each_sharing_interval == 0)    // time to send msg
        {
            State st_send = NextStateToSend(*nit);    // get the next state to be sent to this neighbour

            dbgmoreprt("***", "%d sent state %ld to %d\n", id, st_send, *nit);
            SendStateInfo(*nit, st_send);    // send out
        }
    }
}

/**
 * \brief Join a sharing network
 * \param cn sharing network to join
 */
void IAgent::JoinDENet(DENet *cn)
{
    denet = cn;
    denet->AddMember(id);    // add me as a member
}

/**
 * \brief Leave a sharing network
 */
void IAgent::LeaveDENet()
{
// check first
    if (denet == NULL)    // not join in any net
    {
        return;
    }

    denet->RemoveMember(id);    // remove me from network
    denet = NULL;    // set net as null
    return;
}

void IAgent::AddNeighbour(int nid, int interval)
{
// chech if joined in any network
    if (denet == NULL)
    {
        WARNNING(
                "AddNeighbour(): agent %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return;
    }

    denet->AddNeighbour(id, nid, interval);
}

void IAgent::ChangeExchangeInterval(int nid, int newinterval)
{
    // chech if joined in any network
    if (denet == NULL)
    {
        WARNNING("AddNeighbour(): agent %d hasn't joint any network yet!\n",
                id);
        return;
    }

    denet->ChangeExchangeInterval(id, nid, newinterval);
}

void IAgent::RemoveNeighbour(int nid)
{
// chech if joined in any network
    if (denet == NULL)
    {
        WARNNING(
                "RemoveNeighbour(): agent %d hasn't joint any network yet, it has no neighbour to remove!\n",
                id);
        return;
    }

    denet->RemoveNeighbour(id, nid);
}

int IAgent::GetExchangeInterval(int neb)
{
// chech if joined in any network
    if (denet == NULL)
    {
        WARNNING(
                "AddNeighbour(): agent %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return INT_MAX;
    }

    return denet->GetExchangeInterval(id, neb);
}

std::set<int> IAgent::GetMyNeighbours()
{
// chech if joined in any network
    if (denet == NULL)
    {
        WARNNING(
                "GetMyNeighbours(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return std::set<int>();
    }

    return denet->GetNeighbours(id);
}

bool IAgent::CheckNeighbourShip(int nid)
{
// chech if joined in any network
    if (denet == NULL)
    {
        WARNNING(
                "CheckNeighbour(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return false;
    }

    return denet->CheckNeighbourShip(id, nid);
}

/**
 * \brief Send information of a specified state to a neighbour.
 * \param st state value to be sent
 */
void IAgent::SendStateInfo(int toneb, IAgent::State st)
{
    if (denet == NULL)    // no neighbours, nothing to do
        return;

    struct State_Info_Header *stif = NULL;
    stif = GetStateInfo(st);    // the st may not exist
    if (stif == NULL)
    {
        return;
    }

    denet->Send(id, toneb, stif, stif->size);    // call the send facility in ienet
    free(stif);    // free

    return;
}

/**
 * \brief Recieve state information from neighbours.
 */
void IAgent::RecvStateInfo()
{
    if (denet == NULL)    // no neighbours, nothing to do
        return;

    char re_buf[2048];    // buffer for recieved message

    if (denet->Recv(id, -1, re_buf, 2048) != 0)    // fetch one message from any agent
    {
        MergeStateInfo((struct State_Info_Header *) re_buf);    // merge the recieved state information to memory
    }
    return;
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void IAgent::PrintStateInfo(const struct State_Info_Header *stif)
{
    if (stif == NULL) return;

    int i = 0;
    printf("======================= State: %ld ===========================\n",
            stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n",
            stif->original_payoff, stif->payoff, stif->count);
    printf("------------------- ExActions, Num: %d ------------------------\n",
            stif->eat_num);
    unsigned char *p = (unsigned char *) stif;
    p += sizeof(struct State_Info_Header);
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;
    for (i = 0; i < stif->eat_num; i++)
    {
        printf("\t EnvAction: %ld,\t\t Count: %ld\n", eaif[i].eat,
                eaif[i].count);
    }
    printf("--------------------- Actions, Num: %d -----------------------\n",
            stif->act_num);
    int len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct Action_Info *atif = (struct Action_Info *) p;
    for (i = 0; i < stif->act_num; i++)
    {
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", atif[i].act,
                atif[i].payoff);
    }
    printf(
            "----------------------- ForwardLinks, Num: %d -------------------------\n",
            stif->lk_num);
    len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct Forward_Link *lk = (struct Forward_Link *) p;
    for (i = 0; i < stif->lk_num; i++)
    {
        printf("\t ForwardLink:\t\t  .|+++ %ld +++ %ld ++> %ld\n", lk[i].eat,
                lk[i].act, lk[i].nst);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

