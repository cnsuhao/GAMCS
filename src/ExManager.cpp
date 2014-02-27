// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <set>
#include <math.h>
#include <string.h>
#include "gimcs/ExNet.h"
#include "gimcs/MAgent.h"
#include "gimcs/ExManager.h"

namespace gimcs
{

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

void ExManager::ConnectMAgent(MAgent *agent)
{
    ConnectAgent(dynamic_cast<Agent *>(agent));
    magent = agent;
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

std::set<int> ExManager::GetMyNeighbours() const
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

bool ExManager::CheckNeighbourShip(int nid) const
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
    return Ava_GetCurrentState();
}

inline void ExManager::PerformAction(Agent::Action act)
{
    return Ava_PerformAction(act);
}

inline OSpace ExManager::ActionCandidates(Agent::State st)
{
    // check exps
    if (ava_loop_count % cps == 0)    // time to stop avatar and exchange memory
    {
        return OSpace();
    }

    OSpace outputs = Ava_ActionCandidates(st);
    if (outputs.Empty()) quit = true;    // quit when avatar quits
    return outputs;
}

inline float ExManager::OriginalPayoff(Agent::State st)
{
    return Ava_OriginalPayoff(st);
}

float ExManager::Ava_OriginalPayoff(Agent::State st)
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
        struct State_Info_Header *my_state = magent->GetStateInfo(re_state->st);
        if (my_state != NULL)
        {
            struct State_Info_Header *merged_state = MergeStateInfo(my_state,
                    re_state);
            magent->UpdateStateInfo(merged_state);    // merge the recieved state information to memory
            free(my_state);
            free(merged_state);
        }
        else
        {
            magent->AddStateInfo(re_state);
        }

    }
    return;
}

struct State_Info_Header *ExManager::MergeStateInfo(
        const struct State_Info_Header *origsthd,
        const struct State_Info_Header *recvsthd) const
{
//    if (origsthd->st != recvsthd->st)
//    {
//        WARNNING(
//                "MergeStateInfo(): state value dones't match, one is %ld, the other is %ld, this shouldn't happen!\n",
//                origsthd->st, recvsthd->st);
//        return NULL;
//    }
//
//#ifdef _DEBUG_MORE_
//    printf(
//            "*************************** merge %ld to %ld ********************************\n",
//            recvsthd->st, origsthd->st);
//    PrintStateInfo(recvsthd);
//    PrintStateInfo(origsthd);
//#endif
//    /* numbers */
//    int eat_num = 0;    // number of envir actions
//    int act_num = 0;    // number of actions
//    int lk_num = 0;    // number of forward links
//
//    // allocate big enough buffers to store temporal env actions, actions and links
//    unsigned char *eat_buf = (unsigned char *) malloc(
//            (origsthd->eat_num + recvsthd->eat_num) * sizeof(EnvAction_Info));
//    unsigned char *act_buf = (unsigned char *) malloc(
//            (origsthd->act_num + recvsthd->act_num) * sizeof(Action_Info));
//    unsigned char *lk_buf = (unsigned char *) malloc(
//            (origsthd->lk_num + recvsthd->lk_num) * sizeof(Forward_Link_Info));
//
//    // point to traverse state info and buffers
//    unsigned char *ptr1 = (unsigned char *) origsthd;
//    unsigned char *ptr2 = (unsigned char *) recvsthd;
//    unsigned char *buf_ptr;
//
//    /* fill env action to eat_buf */
//    ptr1 += sizeof(struct State_Info_Header);
//    ptr2 += sizeof(struct State_Info_Header);
//    buf_ptr = eat_buf;
//
//    struct EnvAction_Info *eaif1, *eaif2;
//    // halve each eat count firstly
//    for (int i = 0; i < origsthd->eat_num; i++)
//    {
//        eaif1 = ((struct EnvAction_Info *) ptr1) + i;
//        eaif1->count = round(eaif1->count / 2.0);
//    }
//
//    for (int i = 0; i < recvsthd->eat_num; i++)
//    {
//        eaif2 = ((struct EnvAction_Info *) ptr2) + i;
//        eaif2->count = round(eaif2->count / 2.0);
//    }
//
//    // then add up two halves
//    for (int i = 0; i < origsthd->eat_num; i++)
//    {
//        eaif1 = ((struct EnvAction_Info *) ptr1) + i;
//        int j;
//        for (j = 0; j < recvsthd->eat_num; j++)
//        {
//            eaif2 = ((struct EnvAction_Info *) ptr2) + j;
//
//            // compare
//            if (eaif2->eat == eaif1->eat)
//            {
//                eaif2->count += eaif1->count;
//                break;
//            }
//        }
//
//        if (j == recvsthd->eat_num)    // not found, it's a tostif only eat, copy it to eat_buf
//        {
//            eat_num++;
//            memcpy(buf_ptr, eaif1, sizeof(EnvAction_Info));
//            buf_ptr += sizeof(EnvAction_Info);
//        }
//    }
//
//    // copy all eat in recvsthd
//    memcpy(buf_ptr, ptr2, recvsthd->eat_num * sizeof(EnvAction_Info));
//    eat_num += recvsthd->eat_num;    // inc number
//
//    /* fill action information */
//    ptr1 += origsthd->eat_num * sizeof(EnvAction_Info);
//    ptr2 += recvsthd->eat_num * sizeof(EnvAction_Info);
//    buf_ptr = act_buf;
//
//    struct Action_Info *acif1, *acif2;
//    for (int i = 0; i < origsthd->act_num; i++)
//    {
//        acif1 = ((struct Action_Info *) ptr1) + i;
//        int j;
//        for (j = 0; j < recvsthd->act_num; j++)
//        {
//            acif2 = ((struct Action_Info *) ptr2) + j;
//
//            // compare
//            if (acif1->act == acif2->act)    // use recvsthd's action payoff, nothing to do
//            {
//                break;
//            }
//        }
//
//        if (j == recvsthd->act_num)    // not found, it's a tostif only act, copy it to act_buf
//        {
//            act_num++;
//            memcpy(buf_ptr, acif1, sizeof(Action_Info));
//            buf_ptr += sizeof(Action_Info);
//        }
//    }
//
//    // copy all act in recvsthd
//    memcpy(buf_ptr, ptr2, recvsthd->act_num * sizeof(Action_Info));
//    act_num += recvsthd->act_num;
//
//    /* forward link information */
//    ptr1 += origsthd->act_num * sizeof(Action_Info);
//    ptr2 += recvsthd->act_num * sizeof(Action_Info);
//    buf_ptr = lk_buf;
//
//    struct Forward_Link_Info *lk1, *lk2;
//    for (int i = 0; i < origsthd->lk_num; i++)
//    {
//        lk1 = ((struct Forward_Link_Info *) ptr1) + i;
//        int j;
//        for (j = 0; j < recvsthd->lk_num; j++)
//        {
//            lk2 = ((struct Forward_Link_Info *) ptr2) + j;
//
//            // compare
//            if (lk1->act == lk2->act && lk1->eat == lk2->eat)    // use recvsthd's next state, nothging to do
//            {
//                break;
//            }
//        }
//
//        if (j == recvsthd->lk_num)    // not found, it's a tostif only link, copy it to lk_buf
//        {
//            lk_num++;
//            memcpy(buf_ptr, lk1, sizeof(Forward_Link_Info));
//            buf_ptr += sizeof(Forward_Link_Info);
//        }
//    }
//
//    // copy all links in recvsthd
//    memcpy(buf_ptr, ptr2, recvsthd->lk_num * sizeof(Forward_Link_Info));
//    lk_num += recvsthd->lk_num;
//
//    // allocate memory
//    int stif_size = sizeof(struct State_Info_Header)
//            + eat_num * sizeof(struct EnvAction_Info)
//            + act_num * sizeof(struct Action_Info)
//            + lk_num * sizeof(struct Forward_Link_Info);
//
//    struct State_Info_Header *stif = (struct State_Info_Header *) malloc(
//            stif_size);
//
//    // fill the header
//    stif->st = recvsthd->st;
//    stif->original_payoff = recvsthd->original_payoff;
//    stif->payoff = recvsthd->payoff;
//    stif->count = round((origsthd->count + recvsthd->count) / 2.0);
//    stif->eat_num = eat_num;
//    stif->act_num = act_num;
//    stif->lk_num = lk_num;
//    stif->size = stif_size;
//
//    // copy all buffers to stif
//    unsigned char *ptr = (unsigned char *) stif;
//    ptr += sizeof(struct State_Info_Header);
//    memcpy(ptr, eat_buf, eat_num * sizeof(EnvAction_Info));
//    free(eat_buf);
//    ptr += eat_num * sizeof(EnvAction_Info);
//    memcpy(ptr, act_buf, act_num * sizeof(Action_Info));
//    free(act_buf);
//    ptr += act_num * sizeof(Action_Info);
//    memcpy(ptr, lk_buf, lk_num * sizeof(Forward_Link_Info));
//    free(lk_buf);
//
//#ifdef _DEBUG_MORE_
//    printf(
//            "----------------------------------------------------------------------------\n");
//    PrintStateInfo(stif);
//    printf(
//            "****************************** merge end **********************************\n\n");
//#endif
    return NULL;
}

void ExManager::SendStateInfo(int toneb, Agent::State st) const
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

}    // namespace gimcs
