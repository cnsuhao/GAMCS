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

CSThreadAvatar::CSThreadAvatar(int i) :
        Avatar(i)
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

