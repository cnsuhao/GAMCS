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
#include "Avatar.h"
#include "Debug.h"

Avatar::Avatar() :
        name("unnamed avatar"), sps(60), myagent(NULL), control_step_time(
                (1000 / sps))
{
}

Avatar::Avatar(std::string n) :
        name(n), sps(60), myagent(NULL), control_step_time(
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
    while (true)
    {
        dbgmoreprt("Enter Launch Loop ", "----------------------------------------------------------- count == %d\n", count);

        unsigned long start_time = GetCurrentTime();

        /* Perceive the outside world */
        Agent::State cs = GetCurrentState();    // get current state
        dbgprt("Launch():", "%s, State: %ld\n", name.c_str(), cs);

        /* Process stage */
        std::vector<Agent::Action> acts = ActionCandidates(cs);    // get all action candidates of a state

        Agent::Action act = myagent->Process(cs, acts);    // choose an action from candidates
        // check validation
        if (act == INVALID_ACTION)    // no valid actions available, reach a dead end, quit. !!!: be sure to check this before update stage
            break;// exit point here

        /* Update stage */
        float oripayoff = OriginalPayoff(cs);    // get original payoff of a state
        myagent->Update(oripayoff);    // agent update inner states

        /* Commmunication */
        myagent->Communicate();

        /* Perform action to the outside world */
        PerformAction(act);    // otherwise, perform the action

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

/**
 * \brief Join a communication network
 * \param grp communication network to join
 */
void Avatar::JoinCommNet(CommNet *cn)
{
    myagent->JoinCommNet(cn);
    ActualJoinCommNet(cn);
}

/**
 * \brief Leave a communication network
 */
void Avatar::LeaveCommNet()
{
    myagent->LeaveCommNet();
    ActualLeaveCommmNet();
}

void Avatar::AddNeighbour(int nid, int interval)
{
    myagent->AddNeighbour(nid, interval);
    ActualAddNeighbour(nid, interval);
}

void Avatar::RemoveNeighbour(int nid)
{
    myagent->RemoveNeighbour(nid);
    ActualRemoveNeighbour(nid);
}

void Avatar::ActualJoinCommNet(CommNet *cn)
{
    UNUSED(cn);
    return;
}

void Avatar::ActualLeaveCommmNet()
{
    return;
}

void Avatar::ActualAddNeighbour(int neb, int interval)
{
    UNUSED(neb);
    UNUSED(interval);
    return;
}

void Avatar::ActualRemoveNeighbour(int neb)
{
    UNUSED(neb);
    return;
}

