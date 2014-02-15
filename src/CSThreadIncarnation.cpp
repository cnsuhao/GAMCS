/*
 * CSThreadAvatar.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSThreadIncarnation.h"

CSThreadIncarnation::CSThreadIncarnation()
{
}

CSThreadIncarnation::CSThreadIncarnation(std::string n) :
        Incarnation(n)
{
}

CSThreadIncarnation::~CSThreadIncarnation()
{
}

/**
 * \brief Launch Launch() function in a thread.
 */
pthread_t CSThreadIncarnation::ThreadLaunch()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);    // create a thread, and call the hook

    return tid;
}

void CSThreadIncarnation::ActualJoinDENet(DENet *cn)
{
    UNUSED(cn);
    return;
}

void CSThreadIncarnation::ActualLeaveDENet()
{
    return;
}
