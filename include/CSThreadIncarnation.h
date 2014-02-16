/*
 * CSIoA.h
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#ifndef CSTHREADINCARNATION_H
#define CSTHREADINCARNATION_H
#include <pthread.h>
#include <string>
#include "Incarnation.h"

/**
 * Computer Simulation Incarnation of Agent.
 */
class CSThreadIncarnation: public Incarnation
{
    public:
        CSThreadIncarnation();
        CSThreadIncarnation(std::string);
        virtual ~CSThreadIncarnation();

        pthread_t ThreadLaunch(); /**< if it's a group, launch each of its  members in a thread */

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<Incarnation *>(args)->Launch();
            return NULL;
        }
};

#endif /* CSTHREADINCARNATION_H */
