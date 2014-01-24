/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdio.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "Avatar.h"
#include "Debug.h"

Avatar::Avatar() :
        id(0), freq(100), sps(64), agent(NULL), commnet(NULL), control_step_time(
                (1000 / sps))
{
}

Avatar::Avatar(int i) :
        id(i), freq(100), sps(64), agent(NULL), commnet(NULL), control_step_time(
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
        if (count >= freq)    // check if it's time to send a message
        {
            dbgmoreprt("", "Count reach %d, send state info...\n", freq);
            SendStateInfo(cs);
            count = 0;    // reset count
        }
        else
            count++;    // inc count

        // handle time related job
        unsigned long end_time = GetCurrentTime();
        unsigned long consumed_time = end_time - start_time;
        long time_remaining = control_step_time - consumed_time;
        if (time_remaining > 0)    // remaining time
        {
            dbgprt("",
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
        } dbgmoreprt("","\n");
    }
    // quit
    dbgmoreprt("Exit Launch Loop", "----------------------------------------------------------- Id: %d Exit!\n", id);
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
