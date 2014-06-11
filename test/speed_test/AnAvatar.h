/*
 * AnAvatar.h
 *
 *  Created on: Mar 31, 2014
 *      Author: andy
 */

#ifndef ANAVATAR_H_
#define ANAVATAR_H_
#include "gamcs/Avatar.h"

using namespace gamcs;

class AnAvatar: public Avatar
{
    public:
        AnAvatar() :
                current_state(0)
        {
        }

        ~AnAvatar()
        {
        }

    private:
        Agent::State current_state;

        Agent::State perceiveState()
        {
            printf("current state: %" ST_FMT "\n", current_state);
            return current_state;
        }

        void performAction(Agent::Action act)
        {
            current_state += act;
        }

        OSpace availableActions(Agent::State)
        {
            OSpace acts;
            acts.add(1);
            return acts;
        }
};

#endif /* ANAVATAR_H_ */
