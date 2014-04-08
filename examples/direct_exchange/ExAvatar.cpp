// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
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
#include "gamcs/OSAgent.h"
#include "gamcs/StateInfoParser.h"
#include "ExAvatar.h"
#include "ExNetwork.h"

ExAvatar::ExAvatar() :
        osagent(NULL), exnet(NULL), cps(10)
{
}

ExAvatar::ExAvatar(int i) :
        Avatar(i), osagent(NULL), exnet(NULL), cps(10)
{
}

ExAvatar::~ExAvatar()
{
}

void ExAvatar::connectOSAgent(OSAgent *osa)
{
    connectAgent(dynamic_cast<Agent *>(osa));
    osagent = osa;
}

int ExAvatar::exStep()
{
    if (ava_loop_count % cps == 0)    // time to stop avatar and exchange memory
        exchange();

    return step();
}

void ExAvatar::exchange()
{
    if (exnet == NULL) return;

    recvStateInfo();

    std::set<int> my_neighbours = getMyNeighbours();
    for (std::set<int>::iterator nit = my_neighbours.begin();
            nit != my_neighbours.end(); ++nit)
    {
        Agent::State st_send = osagent->nextState();
        if (st_send == Agent::INVALID_STATE) st_send = osagent->firstState();

        if (st_send != Agent::INVALID_STATE) sendStateInfo(*nit, st_send);
    }
}

void ExAvatar::joinExNet(ExNetwork *net)
{
    exnet = net;
    exnet->addMember(id);
}

void ExAvatar::leaveExNet()
{
    if (exnet == NULL) return;

    exnet->removeMember(id);
    exnet = NULL;
    return;
}

void ExAvatar::addNeighbour(int nid)
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): agent %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return;
    }

    exnet->addNeighbour(id, nid);
}

void ExAvatar::removeNeighbour(int nid)
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "RemoveNeighbour(): agent %d hasn't joint any network yet, it has no neighbour to remove!\n",
                id);
        return;
    }

    exnet->removeNeighbour(id, nid);
}

std::set<int> ExAvatar::getMyNeighbours() const
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "GetMyNeighbours(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return std::set<int>();
    }

    return exnet->getNeighbours(id);
}

bool ExAvatar::checkNeighbourShip(int nid) const
{
// chech if joined in any network
    if (exnet == NULL)
    {
        WARNNING(
                "CheckNeighbour(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return false;
    }

    return exnet->checkNeighbourShip(id, nid);
}

void ExAvatar::recvStateInfo()
{
    char re_buf[2048];    // buffer for recieved message

    if (exnet->recv(id, -1, re_buf, 2048) != 0)    // fetch one message from any agent
    {
        dbgmoreprt("***", "%d recv from anyone\n", id);

        struct State_Info_Header *re_state = (struct State_Info_Header *) re_buf;

        osagent->connect();    // connect to agent for get/add state info
        struct State_Info_Header *my_state = osagent->getStateInfo(
                re_state->st);
        if (my_state != NULL)
        {
            struct State_Info_Header *merged_state = mergeStateInfo(my_state,
                    re_state);
            osagent->updateStateInfo(merged_state);    // merge the recieved state information to memory
            osagent->updatePayoff(merged_state->st);    // update payoff
            free(my_state);
            free(merged_state);
        }
        else
        {
            osagent->addStateInfo(re_state);
            osagent->updatePayoff(re_state->st);    // update payoff
        }

        osagent->close();    // close connection
    }
    return;
}

struct State_Info_Header *ExAvatar::mergeStateInfo(
        const struct State_Info_Header *origsthd,
        const struct State_Info_Header *recvsthd) const
{
    if (origsthd->st != recvsthd->st)
    {
        WARNNING(
                "MergeStateInfo(): state value dones't match, one is %" ST_FMT ", the other is %" ST_FMT ", this shouldn't happen!\n",
                origsthd->st, recvsthd->st);
        return NULL;
    }

#ifdef _DEBUG_MORE_
    printf(
            "*************************** merge %" ST_FMT " to %" ST_FMT " ********************************\n",
            recvsthd->st, origsthd->st);
    PrintStateInfo(origsthd);
    PrintStateInfo(recvsthd);
#endif

    // make copy, ensure the incoming arguments will not be changed
    char *origsthd_buf = (char *) malloc(origsthd->size);
    memcpy(origsthd_buf, origsthd, origsthd->size);
    State_Info_Header *tmp_origsthd = (State_Info_Header *) origsthd_buf;
    char *recvsthd_buf = (char *) malloc(recvsthd->size);
    memcpy(recvsthd_buf, recvsthd, recvsthd->size);
    State_Info_Header *tmp_recvsthd = (State_Info_Header *) recvsthd_buf;

    char **act_buffer = (char **) malloc((tmp_origsthd->act_num + tmp_recvsthd->act_num) * (tmp_origsthd->size
            + tmp_recvsthd->size));    // buffer for manipulating act info, make sure it's big enough
    int act_num = 0;    // total number of acts

    /********* halve eat count first **********/
    Action_Info_Header *achd = NULL;
    EnvAction_Info *eaif = NULL;
    // tmp_origsthd
    StateInfoParser oparser(tmp_origsthd);
    achd = oparser.firstAct();
    while (achd != NULL)
    {
        eaif = oparser.firstEat();
        while (eaif != NULL)
        {
#ifdef _WIN32_
			eaif->count = int (eaif->count / 2.0 + 0.5);
#else
            eaif->count = round(eaif->count / 2.0);
#endif
			eaif = oparser.nextEat();
        }

        achd = oparser.nextAct();
    }
    // tmp_recvsthd, and copy all acts to buffer
    StateInfoParser rparser(tmp_recvsthd);
    achd = rparser.firstAct();
    while (achd != NULL)
    {
        eaif = rparser.firstEat();
        while (eaif != NULL)
        {
#ifdef _WIN32_
			eaif->count = int (eaif->count / 2.0 + 0.5);
#else
            eaif->count = round(eaif->count / 2.0);
#endif
			eaif = rparser.nextEat();
        }
#ifdef _WIN32_
		memcpy(act_buffer + act_num * (tmp_origsthd->size
            + tmp_recvsthd->size), achd,
                sizeof(Action_Info_Header)
                        + achd->eat_num * sizeof(EnvAction_Info));
		
#else
        memcpy(act_buffer[act_num], achd,
                sizeof(Action_Info_Header)
                        + achd->eat_num * sizeof(EnvAction_Info));
#endif

        act_num++;    // increase act count
        achd = rparser.nextAct();
    }
    /*******************************************/

    unsigned char *buf_acpt = NULL;
    Action_Info_Header *buf_achd = NULL;
    unsigned char *buf_eapt = NULL;
    EnvAction_Info *buf_eaif = NULL;
    // compare each act from tmp_origsthd with acts from tmp_recvsthd
    int tmp_act_num = act_num;    // act_num will be changed
    achd = oparser.firstAct();
    while (achd != NULL)
    {
        int i;
        // traverse all acts in buffer
        for (i = 0; i < tmp_act_num; i++)
        {
            buf_acpt = (unsigned char *) act_buffer[i];
            buf_achd = (Action_Info_Header *) buf_acpt;
            if (buf_achd->act == achd->act)
            {
                // compare each eat from tmp_origsthd with eats from tmp_recvsthd
                int tmp_eat_num = buf_achd->eat_num;    // eat_num will be changed
                eaif = oparser.firstEat();
                while (eaif != NULL)
                {
                    buf_eapt = (buf_acpt + sizeof(Action_Info_Header));    // move to the first eat
                    buf_eaif = (EnvAction_Info *) buf_eapt;
                    int j;
                    // traverse all eats of current act in buffer
                    for (j = 0; j < tmp_eat_num; j++)
                    {
                        if (buf_eaif->eat == eaif->eat)
                        {
                            buf_eaif->count += eaif->count;    // add up eat count
                            break;
                        }

                        buf_eapt += sizeof(EnvAction_Info);    // next eat info
                    }

                    if (j >= tmp_eat_num)    // eat not found, it's a new eat in tmp_origsthd
                    {
                        // append it to current act buffer
                        memcpy(
                                buf_acpt + sizeof(Action_Info_Header)
                                        + buf_achd->eat_num
                                                * sizeof(EnvAction_Info), eaif,
                                sizeof(EnvAction_Info));
                        buf_achd->eat_num++;
                    }

                    eaif = oparser.nextEat();
                }
                break;
            }
        }

        if (i >= tmp_act_num)    // act not found, it's a new act in tmp_origsthd
        {
            // append it to act buffer
            memcpy(act_buffer[act_num], achd,
                    sizeof(Action_Info_Header)
                            + achd->eat_num * sizeof(EnvAction_Info));
            act_num++;
        }

        achd = oparser.nextAct();
    }

    // get total sthd_size
    unsigned int sthd_size = 0;
    sthd_size += sizeof(State_Info_Header);
    for (int i = 0; i < act_num; i++)
    {
        buf_achd = (Action_Info_Header *) act_buffer[i];
        sthd_size += sizeof(Action_Info_Header)
                + buf_achd->eat_num * sizeof(EnvAction_Info);
    }

    State_Info_Header *sthd = (State_Info_Header *) malloc(sthd_size);
    // fill the header
    sthd->st = tmp_recvsthd->st;
    sthd->act_num = act_num;
#ifdef _WIN32_
	sthd->count = int ((tmp_origsthd->count + tmp_recvsthd->count) / 2.0 + 0.5);
#else
	sthd->count = round((tmp_origsthd->count + tmp_recvsthd->count) / 2.0);
#endif
	sthd->payoff = tmp_recvsthd->payoff;
    sthd->original_payoff = tmp_recvsthd->original_payoff;
    sthd->size = sthd_size;

    // copy act info from buffer
    unsigned char *ptr = (unsigned char *) sthd;
    ptr += sizeof(State_Info_Header);    // point to the first act
    unsigned int act_size = 0;
    for (int i = 0; i < act_num; i++)
    {
        buf_achd = (Action_Info_Header *) act_buffer[i];
        act_size = sizeof(Action_Info_Header)
                + buf_achd->eat_num * sizeof(EnvAction_Info);
        memcpy(ptr, buf_achd, act_size);
        ptr += act_size;
    }

#ifdef _DEBUG_MORE_
    printf(
            "------------------------------ merged result -------------------------------------\n");
    PrintStateInfo(sthd);
    printf(
            "****************************** merge end **********************************\n\n");
#endif
	
	// cleanup
	free(origsthd_buf);
	free(recvsthd_buf);

    return sthd;
}

void ExAvatar::sendStateInfo(int toneb, Agent::State st) const
{
    struct State_Info_Header *stif = NULL;
    osagent->connect();
    stif = osagent->getStateInfo(st);    // the st may not exist
    osagent->close();
    if (stif == NULL)
    {
        return;
    }

    dbgmoreprt("***", "%d send %" ST_FMT " to %d\n", id, st, toneb);
    exnet->send(id, toneb, stif, stif->size);    // call the send facility in ienet
    free(stif);    // free

    return;
}

