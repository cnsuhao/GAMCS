/*
 * PrintViewer.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

#include "PrintViewer.h"
#include "Storage.h"

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
    // print memory info
    struct Memory_Info *memif = storage->FetchMemoryInfo();
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
        printf("===========================================================\n\n");
    }
    else
    {
        dbgprt("Show()", " storage is empty.\n");
    }

    // print states info
    Agent::State st;
    unsigned long index = 0;    // load states from database one by one
    while ((st = storage->StateByIndex(index)) != INVALID_STATE)    // get state value
    {
        struct State_Info_Header *stif = storage->FetchStateInfo(st);
        if (stif != NULL)
        {
            PrintStateInfo(stif);
            free(stif);
            index++;
        }
        else
            ERROR("Show(): state: %ld information is NULL!\n", st);
    }
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void PrintViewer::PrintStateInfo(const struct State_Info_Header *stif)
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

