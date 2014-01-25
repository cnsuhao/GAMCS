/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "Avatar.h"
#include "Debug.h"

Avatar::Avatar() :
        id(0), comm_freq(100), sps(60), agent(NULL), commnet(NULL), control_step_time(
                (1000 / sps))
{
}

Avatar::Avatar(int i) :
        id(i), comm_freq(100), sps(60), agent(NULL), commnet(NULL), control_step_time(
                (1000 / sps))
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
    int count = 0;    // count for sending messages

    while (true)
    {
        dbgmoreprt("Enter Launch Loop ", "----------------------------------------------------------- count == %d\n", count);

        unsigned long start_time = GetCurrentTime();

        RecvStateInfo();    // check if new message has recieved

        /* Perceive the outside world */
        Agent::State cs = GetCurrentState();    // get current state
        printf("Avata Id: %d, Current state: %ld\n ", id, cs);

        /* Process stage */
        std::vector<Agent::Action> acts = ActionCandidates(cs);    // get all action candidates of a state

        Agent::Action act = agent->Process(cs, acts);    // choose an action from candidates
        // check validation
        if (act == INVALID_ACTION)    // no valid actions available, reach a dead end, quit. !!!: be sure to check this before update stage
            break;// exit point here

        /* Update stage */
        float oripayoff = OriginalPayoff(cs);    // get original payoff of a state
        agent->Update(oripayoff);    // agent update inner states

        /* Perform action */
        DoAction(act);    // otherwise, perform the action

        /* Commmunication */
        if (count >= comm_freq)    // check if it's time to send a message
        {
            Agent::State state_to_send = agent->StateToSend();
            if (state_to_send != INVALID_STATE)
            {
                dbgmoreprt("", "Count reach %d, send state %ld ...\n", comm_freq, state_to_send);
                SendStateInfo(state_to_send);
                count = 0;    // reset count
            }
        }
        else
            count++;    // inc count

        // handle time related job
        unsigned long end_time = GetCurrentTime();
        unsigned long consumed_time = end_time - start_time;
        long time_remaining = control_step_time - consumed_time;
        if (time_remaining > 0)    // remaining time
        {
            dbgmoreprt("",
                    "You got %ld milliseconds remaining to do other things.\n",
                    time_remaining);
            // do some useful things here if you don't want to sleep
            usleep(time_remaining * 1000);
        }
        else
        {
            WARNNING(
                    "time is not enough to run a step, %ld in lack, try to decrease the sps!\n",
                    -time_remaining);
        }dbgmoreprt("","\n");
    }
    // quit
    dbgmoreprt("Exit Launch Loop", "----------------------------------------------------------- Id: %d Exit!\n", id);
    return;
}


/**
 * \brief Send information of a specified state to all neighbours.
 * \param st state value to be sent
 */
void Avatar::SendStateInfo(Agent::State st)
{
    if (commnet == NULL)    // no neighbours, nothing to do
        return;

    struct State_Info_Header *stif = NULL;
    stif = agent->GetStateInfo(st);    // the st may not exist
    if (stif == NULL)
    {
        return;
    }

    commnet->Send(id, stif, stif->size);    // call the send facility in commnet
    free(stif);    // free

    return;
}


/**
 * \brief Recieve state information from neighbours.
 */
void Avatar::RecvStateInfo()
{
    if (commnet == NULL)    // no neighbours, nothing to do
        return;

    char re_buf[2048];    // buffer for recieved message

    while (commnet->Recv(id, re_buf, 2048) != 0)    // message recieved
    {
        agent->MergeStateInfo((struct State_Info_Header *)re_buf);    // merge the recieved state information to memory
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
    return 1.0;    // original payoff of states is 1.0 by default
}

unsigned long Avatar::GetCurrentTime()
{
    struct timeb tb;
    ftime(&tb);
    return 1000 * tb.time + tb.millitm;
}
