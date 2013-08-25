#include <stdio.h>
#include "Database.h"

int main(void) {
//    int ai_len = 2*sizeof(struct Action_Info);
//    int ea_len = 1*sizeof(struct ExAction_Info);
//    int lk_len = 3*sizeof(struct pLink);
//
//    struct State_Info *stif = (struct State_Info *)malloc(sizeof(struct State_Info) + ai_len + ea_len + lk_len);
//    stif->st = 1;
//    stif->original_payoff = 0.1;
//    stif->payoff = 0.75;
//    stif->count = 198;
//    stif->act_num = 2;
//    stif->eat_num = 1;
//    stif->lk_num = 3;
//    stif->length = sizeof(struct State_Info) + ai_len + ea_len + lk_len;
//
//    stif->atifs[0].act = 1;
//    stif->atifs[0].payoff = 0.2;
//    stif->atifs[1].act = 2;
//    stif->atifs[1].payoff = 0.9;
//
//    stif->belief[0].eat = 4;
//    stif->belief[0].count = 111;
//
//    stif->lks[0].pst = -2;
//    stif->lks[0].pact = 2;
//    stif->lks[0].peat = -1;
//    stif->lks[1].pst = -1;
//    stif->lks[1].pact = 2;
//    stif->lks[1].peat = -1;
//    stif->lks[2].pst = -1;
//    stif->lks[2].pact = 2;
//    stif->lks[2].peat = -1;
//
//    PrintStateInfo(stif);
//    int re = database_connect("localhost", "root", "890127", "MyAgent");
//    if (re == -1) {
//        printf("database connect failed\n");
//        return -1;
//    }
//    printf("Before update\n");
//    add_state_info(stif);
//    struct State_Info *si = fetch_state_info(1);
//    PrintStateInfo(si);

    //database_create();
    int re = database_connect("localhost", "root", "890127", "MyAgent");

    State st;
    while((st = db_next_state()) != -1)
        printf("State: %ld\n", st);
    return 0;
}
