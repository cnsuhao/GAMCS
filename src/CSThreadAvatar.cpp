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


#include <pthread.h>
#include "CSThreadAvatar.h"

namespace gimcs
{

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

}    // namespace gimcs
