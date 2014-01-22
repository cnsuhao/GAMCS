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
void CSSocialAvatar::RecvStateInfo()
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
