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

#include "gimcs/PrintViewer.h"
#include "gimcs/Storage.h"
#include "gimcs/StateInfoParser.h"

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
        printf("last state: \t%" ST_FMT "\n", memif->last_st);
        printf("last action: \t%" ACT_FMT "\n", memif->last_act);
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
            ERROR("Show(): state: %" ST_FMT " information is NULL!\n", st);
    }
    storage->Close();
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void PrintViewer::PrintStateInfo(const struct State_Info_Header *sthd) const
{
    if (sthd == NULL) return;

    printf("++++++++++++++++++++++++ State: %" ST_FMT " ++++++++++++++++++++++++++\n",
            sthd->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld, ActNum: %ld\n",
            sthd->original_payoff, sthd->payoff, sthd->count, sthd->act_num);

    printf("------------------------------------------------------------\n");

    StateInfoParser sparser(sthd);
    Action_Info_Header *athd = NULL;
    EnvAction_Info *eaif = NULL;

    athd = sparser.FirstAct();
    while (athd != NULL)
    {
        eaif = sparser.FirstEat();
        while (eaif != NULL)
        {
            printf("\t  .|+++ %" ACT_FMT " +++ %" ACT_FMT " ++> %" ST_FMT " \t Count: %ld\n", athd->act,
                    eaif->eat, eaif->nst, eaif->count);

            eaif = sparser.NextEat();
        }

        athd = sparser.NextAct();
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
        printf("state %" ST_FMT " not found in memory!\n", st);
    }
    storage->Close();
}

}    // namespace gimcs
