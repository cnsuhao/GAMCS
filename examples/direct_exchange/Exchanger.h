/*
 * messager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MESSAGER_H_
#define MESSAGER_H_
#include "ExAvatar.h"

using namespace gamcs;

class Exchanger: public ExAvatar
{
    public:
        Exchanger() :
                position(5)
        {
        }
        Exchanger(int i) :
                ExAvatar(i), position(5)
        {
        }
        ~Exchanger()
        {
        }

    private:
        Agent::State position;

        Agent::State percieveState()
        {
            printf("Messager %d, State: %" ST_FMT "\n", id, position);
            return position;
        }

        void performAction(Agent::Action act)
        {
            position += act;

            if (position > 15) position = 15;
            if (position < 1) position = 1;
            return;
        }

        OSpace availableActions(Agent::State st)
        {
            OSpace acts;
            acts.clear();
            if (st == 1)    // position 1
            {
                acts.add(1);
                return acts;
            }

            acts.add(1);
            acts.add(-1);
            return acts;
        }

        float originalPayoff(Agent::State st)
        {
            if (st == 9)
                return 1;
            else if (st == 13)
                return 2;
            else
                return 0;

        }

};

#endif /* MESSAGER_H_ */
