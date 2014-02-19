/*
 * CSIoA.h
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */
#ifndef CSTHREADAVATAR_H_
#define CSTHREADAVATAR_H_
#include <pthread.h>
#include <string>
#include "Avatar.h"

namespace gimcs
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

        pthread_t ThreadLaunch(); /**< if it's a group, launch each of its  members in a thread */

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Launch();
            return NULL;
        }
};

}    // namespace gimcs
#endif /* CSTHREADAVATAR_H_ */
