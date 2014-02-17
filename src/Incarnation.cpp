/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <stdlib.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "Incarnation.h"
#include "Debug.h"

Incarnation::Incarnation() :
        name("unnamed avatar"), sps(-1), count(0), myagent(NULL), control_step_time(
                0)
{
}

Incarnation::Incarnation(std::string n) :
        name(n), sps(-1), count(0), myagent(NULL), control_step_time(0)
{
}

Incarnation::~Incarnation()
{
}

/**
 * \brief Launch a avatar continuously.
 */
void Incarnation::Launch()
{
    while (true)
    {
        count++;    // inc count
        dbgmoreprt("Enter Launch Loop ", "----------------------------------------------------------- count == %d\n", count);

        unsigned long start_time = GetCurrentTime();

        /* Perceive the outside world */
        IAgent::State cs = GetCurrentState();    // get current state
        dbgmoreprt("Launch():", "%s, State: %ld\n", name.c_str(), cs);

        /* Process stage */
        OSpace acts = ActionCandidates(cs);    // get all action candidates of a state

        IAgent::Action act = myagent->Process(cs, acts);    // choose an action from candidates
        // check validation
        if (act == INVALID_ACTION)    // no valid actions available, reach a dead end, quit. !!!: be sure to check this before update stage
            break;// exit point here

        /* Update stage */
        float oripayoff = OriginalPayoff(cs);    // get original payoff of a state
        myagent->Update(oripayoff);    // agent update inner states

        /* Perform action to the outside world */
        PerformAction(act);    // otherwise, perform the action

        // handle time related job
        if (sps > 0)    // no control when sps <= 0
        {
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
                        "time is not enough to run a step, %ldms in lack, try to decrease the sps!\n",
                        -time_remaining);
            }
        }
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
float Incarnation::OriginalPayoff(IAgent::State st)
{
    UNUSED(st);
    return 1.0;    // original payoff of states is 1.0 by default
}

unsigned long Incarnation::GetCurrentTime()
{
    struct timeb tb;
    ftime(&tb);
    return 1000 * tb.time + tb.millitm;
}
