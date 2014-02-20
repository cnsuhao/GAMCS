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
//
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


#ifndef CSTHREADEXMANAGER_H
#define CSTHREADEXMANAGER_H
#include <pthread.h>
#include <ExManager.h>

namespace gimcs
{

/**
 *
 */
class CSThreadExManager: public ExManager
{
    public:
        CSThreadExManager();
        CSThreadExManager(int);
        virtual ~CSThreadExManager();

        pthread_t ThreadRun();

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<ExManager *>(args)->Run();
            return NULL;
        }
};

}

#endif /* CSTHREADEXMANAGER_H */
