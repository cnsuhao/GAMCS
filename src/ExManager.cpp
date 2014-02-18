/*
 * ExManager.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#include <set>
#include <math.h>
#include <string.h>
#include "ExNet.h"
#include "MAgent.h"
#include "ExManager.h"

ExManager::ExManager() :
        id(0), magent(NULL), exnet(NULL), cps(10), quit(false)
{
}

ExManager::ExManager(int i) :
        id(i), magent(NULL), exnet(NULL), cps(10), quit(false)
{
}

ExManager::~ExManager()
{
}

void ExManager::ConnectMAgent(MAgent *magent)
{
    ConnectAgent(dynamic_cast<Agent *>(magent));
    magent = magent;
}

void ExManager::Run()
{
    while (!quit)
    {
        // launch or relaunch
        Launch();

        // launch stopped, time to exchange memory
        Exchange();
    }
}

void ExManager::Exchange()
{
    if (exnet == NULL) return;

    RecvStateInfo();

    std::set<int> my_neighbours = GetMyNeighbours();
    for (std::set<int>::iterator nit = my_neighbours.begin();
            nit != my_neighbours.end(); ++nit)
    {
        Agent::State st_send = magent->NextState();
        if (st_send == INVALID_STATE) st_send = magent->FirstState();

        if (st_send != INVALID_STATE) SendStateInfo(*nit, st_send);
    }
}

void ExManager::JoinExNet(ExNet *net)
{
    exnet = net;
    exnet->AddMember(id);
}

void ExManager::LeaveExNet()
{
    if (exnet == NULL) return;

    exnet->RemoveMember(id);
    exnet = NULL;
    return;
}

void ExManager::AddNeighbour(int nid)
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): agent %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return;
    }

    exnet->AddNeighbour(id, nid);
}

void ExManager::RemoveNeighbour(int nid)
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "RemoveNeighbour(): agent %d hasn't joint any network yet, it has no neighbour to remove!\n",
                id);
        return;
    }

    exnet->RemoveNeighbour(id, nid);
}

std::set<int> ExManager::GetMyNeighbours()
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "GetMyNeighbours(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return std::set<int>();
    }

    return exnet->GetNeighbours(id);
}

bool ExManager::CheckNeighbourShip(int nid)
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "CheckNeighbour(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return false;
    }

    return exnet->CheckNeighbourShip(id, nid);
}

inline Agent::State ExManager::GetCurrentState()
{
    return Incar_GetCurrentState();
}

inline void ExManager::PerformAction(Agent::Action act)
{
    return Incar_PerformAction(act);
}

inline OSpace ExManager::ActionCandidates(Agent::State st)
{
    // check exps
    if (incar_loop_count % cps == 0)    // time to stop incarnation and exchange memory
    {
        return OSpace();
    }

    OSpace outputs = Incar_ActionCandidates(st);
    if (outputs.Empty()) quit = true;   // quit when incarnation quits
    return outputs;
}

inline float ExManager::OriginalPayoff(Agent::State st)
{
    return Incar_OriginalPayoff(st);
}

float ExManager::Incar_OriginalPayoff(Agent::State st)
{
    UNUSED(st);
    return 1.0;
}

void ExManager::RecvStateInfo()
{
    char re_buf[2048];    // buffer for recieved message

    if (exnet->Recv(id, -1, re_buf, 2048) != 0)    // fetch one message from any agent
    {
        dbgmoreprt("***", "%d recv from anyone\n", id);

        struct State_Info_Header *re_state = (struct State_Info_Header *) re_buf;
        struct State_Info_Header *my_state = magent->GetStateInfo(
                re_state->st);
        if (my_state != NULL)
        {
            struct State_Info_Header *merged_state = MergeStateInfo(my_state,
                    re_state);
            magent->SetStateInfo(merged_state);    // merge the recieved state information to memory
            free(my_state);
            free(merged_state);
        }
        else
        {
            magent->SetStateInfo(re_state);
        }

    }
    return;
}

struct State_Info_Header *ExManager::MergeStateInfo(
        const struct State_Info_Header *origstif,
        const struct State_Info_Header *recvstif)
{
    if (origstif->st != recvstif->st)
    {
        WARNNING(
                "MergeStateInfo(): state value dones't match, one is %ld, the other is %ld, this shouldn't happen!\n",
                origstif->st, recvstif->st);
        return NULL;
    }

#ifdef _DEBUG_MORE_
    printf(
            "*************************** merge %ld to %ld ********************************\n",
            recvstif->st, origstif->st);
    PrintStateInfo(recvstif);
    PrintStateInfo(origstif);
#endif
    /* numbers */
    int eat_num = 0;    // number of envir actions
    int act_num = 0;    // number of actions
    int lk_num = 0;    // number of forward links

    // allocate big enough buffers to store temporal env actions, actions and links
    unsigned char *eat_buf = (unsigned char *) malloc(
            (origstif->eat_num + recvstif->eat_num) * sizeof(EnvAction_Info));
    unsigned char *act_buf = (unsigned char *) malloc(
            (origstif->act_num + recvstif->act_num) * sizeof(Action_Info));
    unsigned char *lk_buf = (unsigned char *) malloc(
            (origstif->lk_num + recvstif->lk_num) * sizeof(Forward_Link));

    // point to traverse state info and buffers
    unsigned char *ptr1 = (unsigned char *) origstif;
    unsigned char *ptr2 = (unsigned char *) recvstif;
    unsigned char *buf_ptr;

    /* fill env action to eat_buf */
    ptr1 += sizeof(struct State_Info_Header);
    ptr2 += sizeof(struct State_Info_Header);
    buf_ptr = eat_buf;

    struct EnvAction_Info *eaif1, *eaif2;
    // halve each eat count firstly
    for (int i = 0; i < origstif->eat_num; i++)
    {
        eaif1 = ((struct EnvAction_Info *) ptr1) + i;
        eaif1->count = round(eaif1->count / 2.0);
    }

    for (int i = 0; i < recvstif->eat_num; i++)
    {
        eaif2 = ((struct EnvAction_Info *) ptr2) + i;
        eaif2->count = round(eaif2->count / 2.0);
    }

    // then add up two halves
    for (int i = 0; i < origstif->eat_num; i++)
    {
        eaif1 = ((struct EnvAction_Info *) ptr1) + i;
        int j;
        for (j = 0; j < recvstif->eat_num; j++)
        {
            eaif2 = ((struct EnvAction_Info *) ptr2) + j;

            // compare
            if (eaif2->eat == eaif1->eat)
            {
                eaif2->count += eaif1->count;
                break;
            }
        }

        if (j == recvstif->eat_num)    // not found, it's a tostif only eat, copy it to eat_buf
        {
            eat_num++;
            memcpy(buf_ptr, eaif1, sizeof(EnvAction_Info));
            buf_ptr += sizeof(EnvAction_Info);
        }
    }

    // copy all eat in recvstif
    memcpy(buf_ptr, ptr2, recvstif->eat_num * sizeof(EnvAction_Info));
    eat_num += recvstif->eat_num;    // inc number

    /* fill action information */
    ptr1 += origstif->eat_num * sizeof(EnvAction_Info);
    ptr2 += recvstif->eat_num * sizeof(EnvAction_Info);
    buf_ptr = act_buf;

    struct Action_Info *acif1, *acif2;
    for (int i = 0; i < origstif->act_num; i++)
    {
        acif1 = ((struct Action_Info *) ptr1) + i;
        int j;
        for (j = 0; j < recvstif->act_num; j++)
        {
            acif2 = ((struct Action_Info *) ptr2) + j;

            // compare
            if (acif1->act == acif2->act)    // use recvstif's action payoff, nothing to do
            {
                break;
            }
        }

        if (j == recvstif->act_num)    // not found, it's a tostif only act, copy it to act_buf
        {
            act_num++;
            memcpy(buf_ptr, acif1, sizeof(Action_Info));
            buf_ptr += sizeof(Action_Info);
        }
    }

    // copy all act in recvstif
    memcpy(buf_ptr, ptr2, recvstif->act_num * sizeof(Action_Info));
    act_num += recvstif->act_num;

    /* forward link information */
    ptr1 += origstif->act_num * sizeof(Action_Info);
    ptr2 += recvstif->act_num * sizeof(Action_Info);
    buf_ptr = lk_buf;

    struct Forward_Link *lk1, *lk2;
    for (int i = 0; i < origstif->lk_num; i++)
    {
        lk1 = ((struct Forward_Link *) ptr1) + i;
        int j;
        for (j = 0; j < recvstif->lk_num; j++)
        {
            lk2 = ((struct Forward_Link *) ptr2) + j;

            // compare
            if (lk1->act == lk2->act && lk1->eat == lk2->eat)    // use recvstif's next state, nothging to do
            {
                break;
            }
        }

        if (j == recvstif->lk_num)    // not found, it's a tostif only link, copy it to lk_buf
        {
            lk_num++;
            memcpy(buf_ptr, lk1, sizeof(Forward_Link));
            buf_ptr += sizeof(Forward_Link);
        }
    }

    // copy all links in recvstif
    memcpy(buf_ptr, ptr2, recvstif->lk_num * sizeof(Forward_Link));
    lk_num += recvstif->lk_num;

    // allocate memory
    int stif_size = sizeof(struct State_Info_Header)
            + eat_num * sizeof(struct EnvAction_Info)
            + act_num * sizeof(struct Action_Info)
            + lk_num * sizeof(struct Forward_Link);

    struct State_Info_Header *stif = (struct State_Info_Header *) malloc(
            stif_size);

    // fill the header
    stif->st = recvstif->st;
    stif->original_payoff = recvstif->original_payoff;
    stif->payoff = recvstif->payoff;
    stif->count = round((origstif->count + recvstif->count) / 2.0);
    stif->eat_num = eat_num;
    stif->act_num = act_num;
    stif->lk_num = lk_num;
    stif->size = stif_size;

    // copy all buffers to stif
    unsigned char *ptr = (unsigned char *) stif;
    ptr += sizeof(struct State_Info_Header);
    memcpy(ptr, eat_buf, eat_num * sizeof(EnvAction_Info));
    free(eat_buf);
    ptr += eat_num * sizeof(EnvAction_Info);
    memcpy(ptr, act_buf, act_num * sizeof(Action_Info));
    free(act_buf);
    ptr += act_num * sizeof(Action_Info);
    memcpy(ptr, lk_buf, lk_num * sizeof(Forward_Link));
    free(lk_buf);

#ifdef _DEBUG_MORE_
    printf(
            "----------------------------------------------------------------------------\n");
    PrintStateInfo(stif);
    printf(
            "****************************** merge end **********************************\n\n");
#endif
    return stif;
}

void ExManager::SendStateInfo(int toneb, Agent::State st)
{
    struct State_Info_Header *stif = NULL;
    stif = magent->GetStateInfo(st);    // the st may not exist
    if (stif == NULL)
    {
        return;
    }

    dbgmoreprt("***", "%d send %ld to %d\n", id, st, toneb);
    exnet->Send(id, toneb, stif, stif->size);    // call the send facility in ienet
    free(stif);    // free

    return;
}
