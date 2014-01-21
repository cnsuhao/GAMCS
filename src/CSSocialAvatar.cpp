/*
 * CSSocialAvatar.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSSocialAvatar.h"

CSSocialAvatar::CSSocialAvatar()
{
}

CSSocialAvatar::CSSocialAvatar(int i) :
        Avatar(i)
{
}

CSSocialAvatar::~CSSocialAvatar()
{
}

/**
 * \brief Launch Launch() function in a thread.
 */
pthread_t CSSocialAvatar::ThreadLaunch()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);    // create a thread, and call the hook

    return tid;
}

/**
 * \brief Send information of a specified state to all neighbours.
 * \param st state value to be sent
 */
void CSSocialAvatar::SendStateInfo(Agent::State st)
{
    if (commnet == NULL)    // no neighbours, nothing to do
        return;

    char si_buffer[SI_MAX_SIZE];
    int len = agent->GetStateInfo(st, si_buffer);    // the st may not exist
    if (len == -1)
    {
        return;
    }

    commnet->Send(id, si_buffer, len);    // call the send facility in commnet

    return;
}

/**
 * \brief Recieve state information from neighbours.
 */
void CSSocialAvatar::RecvStateInfo()
{
    if (commnet == NULL)    // no neighbours, nothing to do
        return;

    char re_buf[SI_MAX_SIZE];    // buffer for recieved message
    char sd_buf[SI_MAX_SIZE];    // buffer for message to be sent

    while (commnet->Recv(id, re_buf, SI_MAX_SIZE) != 0)    // message recieved
    {
        struct State_Info_Header *stif = (struct State_Info_Header *) re_buf;
        int better = agent->MergeStateInfo(stif);    // merge the recieved state information to memory

        if (better == 0)    // the state information wasn't better than mine and thus not accepted, it's my duty to send out my better information to others
        {
            int len = agent->GetStateInfo(stif->st, sd_buf);    // get my information of the same state
            if (len != -1)
            {
                commnet->Send(id, sd_buf, len);    // send it to all my neighbours
            }
        }
    }
    return;
}
