// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------


#include "PrintViewer.h"
#include "Storage.h"

namespace gimcs
{

PrintViewer::PrintViewer() :
        MemoryViewer()
{
}

PrintViewer::PrintViewer(Storage *sg) :
        MemoryViewer(sg)
{
}

PrintViewer::~PrintViewer()
{
}

void PrintViewer::Show()
{
    int re = storage->Connect();
    if (re != 0)    // connect failed
    {
        WARNNING("PrintViewer Show(): connect to storage failed!\n");
        return;
    }

    // print memory info
    struct Memory_Info *memif = storage->GetMemoryInfo();
    if (memif != NULL)
    {
        printf("\n");
        printf("=================== Memory Information ====================\n");
        printf("discount rate: \t%.2f\n", memif->discount_rate);
        printf("threshold: \t%.2f\n", memif->threshold);
        printf("number of states: \t%ld\n", memif->state_num);
        printf("number of links: \t%ld\n", memif->lk_num);
        printf("last state: \t%ld\n", memif->last_st);
        printf("last action: \t%ld\n", memif->last_act);
        free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
        printf(
                "===========================================================\n\n");
    }
    else
    {
        printf("Memory not found in storage!\n");
        storage->Close();
        return;
    }

    // print states info
    Agent::State st = storage->FirstState();
    while (st != INVALID_STATE)    // get state value
    {
        struct State_Info_Header *stif = storage->GetStateInfo(st);
        if (stif != NULL)
        {
            PrintStateInfo(stif);
            free(stif);
            st = storage->NextState();
        }
        else
            ERROR("Show(): state: %ld information is NULL!\n", st);
    }
    storage->Close();
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void PrintViewer::PrintStateInfo(const struct State_Info_Header *stif) const
{
    if (stif == NULL) return;

    int i = 0;
    printf("++++++++++++++++++++++++ State: %ld ++++++++++++++++++++++++++\n",
            stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n",
            stif->original_payoff, stif->payoff, stif->count);
    printf("------------------- ExActions, Num: %d -----------------------\n",
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
    printf("------------------- ForwardLinks, Num: %d --------------------\n",
            stif->lk_num);
    len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct Forward_Link_Info *lk = (struct Forward_Link_Info *) p;
    for (i = 0; i < stif->lk_num; i++)
    {
        printf("\t ForwardLink:\t\t  .|+++ %ld +++ %ld ++> %ld\n", lk[i].eat,
                lk[i].act, lk[i].nst);
    }
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

void PrintViewer::ShowState(Agent::State st)
{
    int re = storage->Connect();
    if (re != 0)    // connect failed
    {
        WARNNING("PrintViewer ShowState(): connect to storage failed!\n");
        return;
    }

    struct State_Info_Header *stif = storage->GetStateInfo(st);
    if (stif != NULL)
    {
        PrintStateInfo(stif);
        free(stif);
    }
    else
    {
        printf("state %ld not found in memory!\n", st);
    }
    storage->Close();
}

}    // namespace gimcs
