// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <pthread.h>
#include "gamcs/CSThreadExManager.h"

namespace gamcs
{

CSThreadExManager::CSThreadExManager()
{
}

CSThreadExManager::CSThreadExManager(int i) :
        ExManager(i)
{
}

CSThreadExManager::~CSThreadExManager()
{
}

pthread_t CSThreadExManager::threadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}

}    // namespace gamcs
