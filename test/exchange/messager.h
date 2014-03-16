/*
 * messager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MESSAGER_H_
#define MESSAGER_H_
#include "gamcs/CSThreadExManager.h"

using namespace gamcs;

class Messager: public CSThreadExManager
{
    public:
        Messager() :
                position(5), count(0)
        {
        }
        Messager(int i) :
                CSThreadExManager(i), position(5), count(0)
        {
        }
        ~Messager()
        {
        }

    private:
        Agent::State position;
        int count;

        Agent::State ava_getCurrentState()
        {
            printf("Messager %d, State: %" ST_FMT "\n", id, position);
            return position;
        }

        void ava_performAction(Agent::Action act)
        {
            position += act;

            if (position > 15) position = 15;
            if (position < 1) position = 1;
            return;
        }

        OSpace ava_actionCandidates(Agent::State st)
        {
            if (count < 1000)
            {
                OSpace acts;
                acts.clear();
                if (st == 1)    // position 1
                {
                    acts.add(1);
                    count++;
                    return acts;
                }

                acts.add(1);
                acts.add(-1);
                count++;
                return acts;
            }
            else
                return OSpace();    // return an empty list
        }

        float ava_originalPayoff(Agent::State st)
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
