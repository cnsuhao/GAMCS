/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include <pthread.h>
#include <stdio.h>
#include "Avatar.h"
#include "Debug.h"

bool Avatar::quit = false;  // set quit indicator to false

Avatar::Avatar() : id(0), freq(100), agent(NULL), cccnet(NULL)
{
}

Avatar::Avatar(int i) : id(i), freq(100), agent(NULL), cccnet(NULL)
{
}

Avatar::~Avatar()
{
}

/**
* \brief Launch Launch() function in a thread.
*/
pthread_t Avatar::ThreadLaunch()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this); // create a thread, and call the hook

    return tid;
}

/**
* \brief Launch a avatar continuously.
*/
void Avatar::Launch()
{
    int count = 0;  // count for sending messages

    while(!quit)
    {
        RecvStateInfo();    // check if new message has recieved

        /* Perceive the outside world */
        Agent::State cs = GetCurrentState();   // get current state
        printf("Avata Id: %d, Current state: %ld\n", id, cs);

        /* Process stage */
        std::vector<Agent::Action> acts = ActionCandidates(cs);   // get all action candidates of a state

        Agent::Action act = agent->Process(cs, acts);  // choose an action from candidates

        /* Update stage */
        float oripayoff = OriginalPayoff(cs);   // get original payoff of a state
        agent->Update(oripayoff);       // agent update inner states

        /* Perform action */
        if (act == INVALID_ACTION)       // no valid actions available, reach a dead end, quit
            break;
        DoAction(act);      // otherwise, perform the action

        /* Commmunication */
        if (count >= freq)      // check if it's time to send a message
        {
            SendStateInfo(cs);
            count = 0;      // reset count
        }
        else
            count++;        // inc count
    }
    // quit
    dbgmoreprt("Id: %d, Run() Exit!\n", id);
    return;
}

/**
* \brief Set communication frequence.
*/
void Avatar::SetFreq(int fq)
{
    freq = fq;
}

/**
* \brief Connect to an agent.
* \param agt agent to be connected
*/
void Avatar::ConnectAgent(Agent *agt)
{
    agent = agt;
}

/**
* \brief Join a cccnet
* \param grp cccnet to join
*/
void Avatar::SetCCCNet(CCCNet *cn)
{
    cccnet = cn;
}

/**
* \brief Send information of a specified state to all neighbours.
* \param st state value to be sent
*/
void Avatar::SendStateInfo(Agent::State st)
{
    if (cccnet == NULL)  // no neighbours, nothing to do
        return;

    char si_buffer[SI_MAX_SIZE];
    int len = agent->GetStateInfo(st, si_buffer);       // the st may not exist
    if (len == -1)
    {
        return;
    }

    cccnet->Send(id, si_buffer, len);        // call the send facility in cccnet

    return;
}

/**
* \brief Recieve state information from neighbours.
*/
void Avatar::RecvStateInfo()
{
    if (cccnet == NULL)  // no neighbours, nothing to do
        return;

    char re_buf[SI_MAX_SIZE];   // buffer for recieved message
    char sd_buf[SI_MAX_SIZE];   // buffer for message to be sent

    while(cccnet->Recv(id, re_buf, 2048) != 0)   // message recieved
    {
        struct State_Info_Header *stif = (struct State_Info_Header *)re_buf;
        int better = agent->MergeStateInfo(stif);   // merge the recieved state information to memory

        if (better == 0)                // the state information wasn't better than mine and thus not accepted, it's my duty to send out my better information to others
        {
            int len = agent->GetStateInfo(stif->st, sd_buf);    // get my information of the same state
            if (len != -1)
            {
                cccnet->Send(id, sd_buf, len);       // send it to all my neighbours
            }
        }
    }
    return;
}

/** \brief Get original payoff of each state.
 *  Return 1 for every state by default.
 * \param st state identity
 * \return original payoff of st
 *
 */
float Avatar::OriginalPayoff(Agent::State st)
{
    UNUSED(st);
    return 1.0;     // original payoff of states is 1.0 by default
}

