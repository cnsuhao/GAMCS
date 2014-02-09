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
#include "CommNet.h"
#include "Debug.h"

Avatar::Avatar() :
        id(0), sps(60), agent(NULL), commnet(NULL), control_step_time(
                (1000 / sps))
{
}

Avatar::Avatar(int i) :
        id(i), sps(60), agent(NULL), commnet(NULL), control_step_time(
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
    int count = 1;    // loop count

    while (true)
    {
        dbgmoreprt("Enter Launch Loop ", "----------------------------------------------------------- count == %d\n", count);

        unsigned long start_time = GetCurrentTime();

        RecvStateInfo();    // check if new message has recieved

        /* Perceive the outside world */
        Agent::State cs = GetCurrentState();    // get current state
        dbgprt("Avata Id", " %d, Current state: %ld\n ", id, cs);

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
        int each_comm_freq;
        std::set<int> my_neighbours = GetMyNeighbours();    // walk through all neighbours to check frequence
        for (std::set<int>::iterator nit = my_neighbours.begin();
                nit != my_neighbours.end(); ++nit)
        {
            each_comm_freq = GetNeighFreq(*nit);    // get communication freq to this neighbour
            if (count % each_comm_freq == 0)    // time to send msg
            {
                // prepare the state to be sent
                if (states_to_send.find(*nit) == states_to_send.end())    // encounter a new neighbour
                {
                    Agent::State st_send = agent->NextStateToSend(
                            INVALID_STATE);    // msg from beginning
                    states_to_send[*nit] = st_send;    // record progress

                    dbgprt("***", "%d sent state %ld to %d\n", id, st_send,
                            *nit);
                    SendStateInfo(*nit, st_send);    // send out
                }
                else    // this neighbour is an acquaintance
                {
                    Agent::State st_send = agent->NextStateToSend(
                            states_to_send[*nit]);    // get the recorded state
                    states_to_send[*nit] = st_send;    // record new progress

                    dbgprt("***", "%d sent state %ld to %d\n", id, st_send,
                            *nit);
                    SendStateInfo(*nit, st_send);    // send out
                }
            }
        }

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

        count++;    // inc count
    }
    // quit
    dbgmoreprt("Exit Launch Loop", "----------------------------------------------------------- Id: %d Exit!\n", id);
    return;
}

/**
 * \brief Send information of a specified state to all neighbours.
 * \param st state value to be sent
 */
void Avatar::SendStateInfo(int toneb, Agent::State st)
{
    if (commnet == NULL)    // no neighbours, nothing to do
        return;

    struct State_Info_Header *stif = NULL;
    stif = agent->GetStateInfo(st);    // the st may not exist
    if (stif == NULL)
    {
        return;
    }

    commnet->Send(toneb, stif, stif->size);    // call the send facility in commnet
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

    if (commnet->Recv(id, re_buf, 2048) != 0)    // fetch one message
    {
        agent->MergeStateInfo((struct State_Info_Header *) re_buf);    // merge the recieved state information to memory
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

/**
 * \brief Join a communication network
 * \param grp communication network to join
 */
void Avatar::JoinCommNet(CommNet *cn)
{
    commnet = cn;
    commnet->AddMember(id);    // add me as a member
}
/**
 * \brief Leave a communication network
 */
void Avatar::LeaveCommNet()
{
    // check first
    if (commnet == NULL)    // not join in any net
    {
        return;
    }

    commnet->RemoveMember(id);    // remove me from network
    commnet = NULL;    // set net as null
    return;
}

void Avatar::AddNeighbour(int nid, int freq)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): menber %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return;
    }

    commnet->AddNeighbour(id, nid, freq);
}

int Avatar::GetNeighFreq(int neb)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): menber %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return INT_MAX;
    }

    return commnet->GetNeighFreq(id, neb);
}

void Avatar::RemoveNeighbour(int nid)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "RemoveNeighbour(): menber %d hasn't joint any network yet, it has no neighbour to remove!\n",
                id);
        return;
    }

    commnet->RemoveNeighbour(id, nid);
}

std::set<int> Avatar::GetMyNeighbours()
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "GetMyNeighbours(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return std::set<int>();
    }

    return commnet->GetNeighbours(id);
}

bool Avatar::CheckNeighbourShip(int nid)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "CheckNeighbour(): menber %d hasn't joint any network yet, no neighbours at all!\n",
                id);
        return false;
    }

    return commnet->CheckNeighbourShip(id, nid);
}
