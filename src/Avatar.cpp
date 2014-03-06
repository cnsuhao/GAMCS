// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <stdlib.h>
#include <sys/timeb.h>
#include <unistd.h>
#include "gimcs/Avatar.h"
#include "gimcs/Debug.h"

namespace gimcs
{

Avatar::Avatar() :
        name("unnamed avatar"), sps(-1), ava_loop_count(0), myagent(NULL), control_step_time(
                0)
{
}

Avatar::Avatar(std::string n) :
        name(n), sps(-1), ava_loop_count(0), myagent(NULL), control_step_time(
                0)
{
}

Avatar::~Avatar()
{
}

/**
 * \brief launch a avatar continuously.
 */
void Avatar::launch()
{
    while (true)
    {
        ava_loop_count++;    // inc count
        dbgmoreprt("Enter Launch Loop ", "------------------------------ count == %ld\n", ava_loop_count);

        unsigned long start_time = getCurrentTime();

        /* Perceive the outside world */
        Agent::State cs = getCurrentState();    // get current state
        dbgmoreprt("Launch():", "%s, State: %ld\n", name.c_str(), cs);

        /* Process stage */
        OSpace acts = actionCandidates(cs);    // get all action candidates of a state

        Agent::Action act = myagent->process(cs, acts);    // choose an action from candidates
        // check validation
        if (act == Agent::INVALID_ACTION)    // no valid actions available, reach a dead end, quit. !!!: be sure to check this before update stage
            break;// exit point here

        /* update stage */
        float oripayoff = originalPayoff(cs);    // get original payoff of a state
        myagent->update(oripayoff);    // agent update inner states

        /* Perform action to the outside world */
        performAction(act);    // otherwise, perform the action

        // handle time related job
        if (sps > 0)    // no control when sps <= 0
        {
            unsigned long end_time = getCurrentTime();
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
    dbgmoreprt("Exit Launch Loop", "----------------------------------------------------------- %s Exit!\n", name.c_str());
    return;
}

/** \brief Get original payoff of each state.
 *  Return 1 for every state by default.
 * \param st state identity
 * \return original payoff of st
 *
 */
float Avatar::originalPayoff(Agent::State st)
{
    UNUSED(st);
    return 1.0;    // original payoff of states is 1.0 by default
}

unsigned long Avatar::getCurrentTime()
{
    struct timeb tb;
    ftime(&tb);
    return 1000 * tb.time + tb.millitm;
}

}    // namespace gimcs
