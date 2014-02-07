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
        }
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

void Avatar::AddNeighbour(int nid)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): menber %d hasn't joint any network yet, can not add a neighbour!\n",
                id);
        return;
    }

    // check if member exists
    if (!commnet->HasMember(nid))    // member not exists
    {
        WARNNING(
                "AddNeighbour(): Member %d doesn't exist, can not make it as a neighbour!\n",
                nid);
        return;
    }

    commnet->AddNeighbour(id, nid);
}

void Avatar::AddNeighbours(const std::set<int> &neighbours)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "AddNeighbour(): menber %d hasn't joint any network yet, can not add a neighbour!!\n",
                id);
        return;
    }

    // add all neighbours
    for (std::set<int>::const_iterator nit = neighbours.begin();
            nit != neighbours.end(); ++nit)
    {
        // check if member exists
        if (!commnet->HasMember(*nit))    // member not exists
        {
            WARNNING(
                    "AddNeighbour(): Member %d doesn't exist, can not make it as a neighbour!\n",
                    *nit);
            return;
        }

        commnet->AddNeighbour(id, *nit);
    }
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

    // check if member exists
    if (!commnet->HasMember(nid))
    {
        WARNNING(
                "RemoveNeighbour(): Member %d doesn't exist, are you sure it's your neighbour?\n",
                nid);
        return;
    }

    commnet->RemoveNeighbour(id, nid);
}

void Avatar::RemoveNeighbours(const std::set<int> &neighbours)
{
    // chech if joined in any network
    if (commnet == NULL)
    {
        WARNNING(
                "RemoveNeighbour(): menber %d hasn't joint any network yet, it has no neighbours to remove!\n",
                id);
        return;
    }

    // remove all neighbours
    for (std::set<int>::const_iterator nit = neighbours.begin();
            nit != neighbours.end(); ++nit)
    {
        // check if member exists
        if (!commnet->HasMember(*nit))
        {
            WARNNING(
                    "RemoveNeighbour(): Member %d doesn't exist, are you sure it's your neighbour?\n",
                    *nit);
            return;
        }

        commnet->RemoveNeighbour(id, *nit);
    }
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
