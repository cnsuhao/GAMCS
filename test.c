#include "Database.h"
#include "Agent.h"

void PrintStateInfo(struct State_Info *stif)
{
    int i;
    printf("===================== State: %ld =========================\n", stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n", stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions, Num: %d -----------------------\n", stif->act_num);
    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    struct Action_Info *atif = (struct Action_Info *)p;
    for (i=0; i< stif->act_num; i++)
    {
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", atif[i].act, atif[i].payoff);
    }
    printf("------------------- ExActions, Num: %d ------------------------\n", stif->eat_num);
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;
    for (i=0; i<stif->eat_num; i++)
    {
        printf("\t ExAction: %ld,\t\t Count: %ld\n", eaif[i].eat, eaif[i].count);
    }
    printf("------------------------ pLinks, Num: %d ---------------------------\n", stif->lk_num);
    len = stif->eat_num * sizeof(struct ExAction_Info);
    p += len;
    struct pLink *lk = (struct pLink *)p;
    for (i=0; i<stif->lk_num; i++)
    {
        printf("\t pLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", lk[i].pst, lk[i].peat, lk[i].pact);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

int main(void)
{
  int ai_len = 2*sizeof(struct Action_Info);
  int ea_len = 1*sizeof(struct ExAction_Info);
  int lk_len = 3*sizeof(struct pLink);
  
  struct State_Info *stif = (struct State_Info *)malloc(sizeof(struct State_Info) + ai_len + ea_len + lk_len);
  stif->st = 1;
  stif->original_payoff = 0.1;
  stif->payoff = 0.75;
  stif->count = 198;
  stif->act_num = 2;
  stif->eat_num = 1;
  stif->lk_num = 3;
  stif->length = sizeof(struct State_Info) + ai_len + ea_len + lk_len;
  
  stif->atifs[0].act = 1;
  stif->atifs[0].payoff = 0.2;  
  stif->atifs[1].act = 2;
  stif->atifs[1].payoff = 0.4;
  
  stif->belief[0].eat = 2;
  stif->belief[0].count = 111;
  
  stif->lks[0].pst = -1;
  stif->lks[0].pact = 2;
  stif->lks[0].peat = -1;
  stif->lks[1].pst = -1;
  stif->lks[1].pact = 2;
  stif->lks[1].peat = -1;
  stif->lks[2].pst = -1;
  stif->lks[2].pact = 2;
  stif->lks[2].peat = -1;
  
  PrintStateInfo(stif);
  return 0;
}