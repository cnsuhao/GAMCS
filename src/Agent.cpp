/**********************************************************************

 *	@File:
 *	@Created: 2013-8-19
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdio.h>
#include "Agent.h"

Agent::Agent() :
        discount_rate(0.8), threshold(0.01), unseen_state_payoff(0.0), unseen_action_payoff(
                0.0), unseen_eaction_maxpayoff(0.0)
{
}

Agent::Agent(float dr, float th) :
        discount_rate(dr), threshold(th), unseen_state_payoff(0.0), unseen_action_payoff(
                0.0), unseen_eaction_maxpayoff(0.0)
{
}

Agent::~Agent()
{
}

/** \brief Restrict capacity of an agent.
 *  Comply with maximun payoff rule.
 * \param st state identity
 * \param acts all possible actions of st
 * \return action distribution after appling maximun payoff restrict
 *
 */
std::vector<Agent::Action> Agent::Restrict(Agent::State st,
        const std::vector<Agent::Action> &acts)
{
    return MaxPayoffRule(st, acts);
}

/** \brief Update inner states.
 *
 */

void Agent::Update(float oripayoff)
{
    UpdateMemory(oripayoff);    // update memory
    TSGIOM::Update();
    return;
}

/**
 * \brief Pretty print State information
 * \param specified State information header
 */
void Agent::PrintStateInfo(const struct State_Info_Header *stif)
{
    if (stif == NULL) return;

    int i = 0;
    printf("======================= State: %ld ===========================\n",
            stif->st);
    printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %ld\n",
            stif->original_payoff, stif->payoff, stif->count);
    printf("--------------------- Actions, Num: %d -----------------------\n",
            stif->act_num);
    unsigned char *p = (unsigned char *) stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;
    for (i = 0; i < stif->act_num; i++)
    {
        printf("\t Action: %ld,\t\t Payoff: %.2f\n", atif[i].act,
                atif[i].payoff);
    }
    printf("------------------- ExActions, Num: %d ------------------------\n",
            stif->eat_num);
    int len = stif->act_num * sizeof(struct Action_Info);
    p += len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;
    for (i = 0; i < stif->eat_num; i++)
    {
        printf("\t EnvAction: %ld,\t\t Count: %ld\n", eaif[i].eat,
                eaif[i].count);
    }
    printf(
            "------------------------ BackLinks, Num: %d ---------------------------\n",
            stif->lk_num);
    len = stif->eat_num * sizeof(struct EnvAction_Info);
    p += len;
    struct BackLink *lk = (struct BackLink *) p;
    for (i = 0; i < stif->lk_num; i++)
    {
        printf("\t BackLink:\t\t %ld |+++ %ld +++ %ld ++>.\n", lk[i].pst,
                lk[i].peat, lk[i].pact);
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

    return;
}

