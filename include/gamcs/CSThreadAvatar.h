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
//
// Created on: Jan 20, 2014
//
// -----------------------------------------------------------------------------


#ifndef CSTHREADAVATAR_H_
#define CSTHREADAVATAR_H_
#include <pthread.h>
#include <string>
#include "gamcs/Avatar.h"

namespace gamcs
{

/**
 * Computer Simulation Avatar of Agent.
 */
class CSThreadAvatar: public Avatar
{
    public:
        CSThreadAvatar();
        CSThreadAvatar(std::string);
        virtual ~CSThreadAvatar();

        pthread_t threadLaunch(); /**< if it's a group, launch each of its  members in a thread */

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(launch() here) in a thread */
            reinterpret_cast<Avatar *>(args)->launch();
            return NULL;
        }
};

}    // namespace gamcs
#endif /* CSTHREADAVATAR_H_ */
