/*
 * CSSocialAvatar.h
 *
 *  Created on: Jan 20, 2014
 *      Author: andy
 */

#ifndef CSSOCIALAVATAR_H_
#define CSSOCIALAVATAR_H_
#include "Avatar.h"

/**
 * Computer Simulation Social Avatar Interface
 */
class CSSocialAvatar : public Avatar
{
    public:
        CSSocialAvatar();
        CSSocialAvatar(int);
        virtual ~CSSocialAvatar();

        pthread_t ThreadLaunch();      /**< if it's a group, launch each of its  members in a thread */
    private:
        virtual void SendStateInfo(Agent::State);      /**< send information of a state to all its neighbours */
        virtual void RecvStateInfo();           /**< recieve state information from neighbours */

        static void* hook(void* args) {         /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Launch();
            return NULL;
        }
};

#endif /* CSSOCIALAVATAR_H_ */
