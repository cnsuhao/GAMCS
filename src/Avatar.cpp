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

Avatar::Avatar() : id(0), freq(100), agent(NULL), group(NULL)
{
}

Avatar::Avatar(int i) : id(i), freq(100), agent(NULL), group(NULL)
{
}

Avatar::~Avatar()
{
}

/**
* \brief Run Run() function in a thread.
*/
pthread_t Avatar::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this); // create a thread, and call the hook

    return tid;
}

/**
* \brief Run a avatar continuously.
*/
void Avatar::Run()
{
    int count = 0;  // count for sending messages

    while(!quit)
    {
        RecvStateInfo();    // check if new message has recieved

        Agent::State cs = GetCurrentState();   // get current state
        printf("Id: %d, Current state: %ld\n", id, cs);

//============= Process stage ===========
        std::vector<Agent::Action> acts = ActionCandidates(cs);   // get all action candidates of a state

        Agent::Action act = agent->Process(cs, acts);  // choose an action from candidates

//============= Update stage ============
        float oripayoff = OriginalPayoff(cs);   // get original payoff of a state
        agent->Update(oripayoff);

        if (act == INVALID_ACTION)       // no valid actions available, reach a dead end, quit
            break;
        DoAction(act);      // otherwise, perform the action

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
    return;
}

/**
* \brief Connect to an agent.
* \param agt agent to be connected
*/
void Avatar::ConnectAgent(Agent *agt)
{
    agent = agt;
    return;
}

/**
* \brief Join a group
* \param grp group to join
*/
void Avatar::JoinGroup(Group *grp)
{
    group = grp;
    return;
}

/**
* \brief Send information of a specified state to all neighbours.
* \param st state value to be sent
*/
void Avatar::SendStateInfo(Agent::State st)
{
    if (group == NULL)  // no neighbours, nothing to do
        return;

    char si_buffer[SI_MAX_SIZE];
    int len = agent->GetStateInfo(st, si_buffer);       // the st may not exist
    if (len == -1)
    {
        return;
    }

    group->Send(id, si_buffer, len);        // call the send facility in group

    return;
}

/**
* \brief Recieve state information from neighbours.
*/
void Avatar::RecvStateInfo()
{
    if (group == NULL)  // no neighbours, nothing to do
        return;

    char re_buf[SI_MAX_SIZE];   // buffer for recieved message
    char sd_buf[SI_MAX_SIZE];   // buffer for message to be sent

    while(group->Recv(id, re_buf, 2048) != 0)   // message recieved
    {
        struct State_Info_Header *stif = (struct State_Info_Header *)re_buf;
        int better = agent->MergeStateInfo(stif);   // merge the recieved state information to memory

        if (better == 0)                // the state information wasn't better than mine and thus not accepted, it's my duty to send out my better information to others
        {
            int len = agent->GetStateInfo(stif->st, sd_buf);    // get my information of the same state
            if (len != -1)
            {
                group->Send(id, sd_buf, len);       // send it to all my neighbours
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
    return 1.0;
}

