/*
 * CSThreadAvatar.h
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#ifndef CSTHREADAVATAR_H
#define CSTHREADAVATAR_H
#include <pthread.h>
#include "Avatar.h"

/**
 * Interface of avatar simulated as a thread running in computer.
 */
class CSThreadAvatar : public Avatar
{
    public:
        CSThreadAvatar();
        CSThreadAvatar(int);
        virtual ~CSThreadAvatar();

        pthread_t ThreadLaunch();      /**< if it's a group, launch each of its  members in a thread */
    private:
        static void* hook(void* args) {         /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Launch();
            return NULL;
        }
};

#endif /* CSTHREADAVATAR_H */
