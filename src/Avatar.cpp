/**********************************************************************
*	@File:
*	@Created: 2013
*	@Author: AndyHuang - andyspider@126.coom
*	@Description:
*
*	@Modify date:
***********************************************************************/
#include <stdio.h>
#include "Avatar.h"
#include "Debug.h"

Avatar::Avatar() : id(0), freq(100), agent(NULL), commnet(NULL)
{
}

Avatar::Avatar(int i) : id(i), freq(100), agent(NULL), commnet(NULL)
{
}

Avatar::~Avatar()
{
}

/**
* \brief Launch a avatar continuously.
*/
void Avatar::Launch()
{
    int count = 0;  // count for sending messages

    while(true)
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

