/*
 * CSThreadAvatar.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSThreadAvatar.h"

CSThreadAvatar::CSThreadAvatar()
{
}

CSThreadAvatar::CSThreadAvatar(std::string n) :
        Avatar(n)
{
}

CSThreadAvatar::~CSThreadAvatar()
{
}

/**
 * \brief Launch Launch() function in a thread.
 */
pthread_t CSThreadAvatar::ThreadLaunch()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);    // create a thread, and call the hook

    return tid;
}

void CSThreadAvatar::ActualJoinParallelNet(ParallelNet *cn)
{
    UNUSED(cn);
    return;
}

void CSThreadAvatar::ActualLeaveParallelNet()
{
    return;
}
