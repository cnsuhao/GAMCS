/*
 * messager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MESSAGER_H_
#define MESSAGER_H_
#include "MMessager.h"

class Msger: public MMessager
{
    public:
        Msger() :
        position(5), count(0)
        {
        }
        Msger(int i) :
        MMessager(i), position(5), count(0)
        {
        }
        ~Msger()
        {
        }

    private:
        IAgent::State position;
        int count;

        IAgent::State Incar_GetCurrentState()
        {
            printf("Messager %d, State: %ld\n", id, position);
            return position;
        }

        void Incar_PerformAction(IAgent::Action act)
        {
            position += act;

            if (position > 15) position = 15;
            if (position < 1) position = 1;
            return;
        }

        OSpace Incar_ActionCandidates(IAgent::State st)
        {
            if (count < 500)
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

        float Incar_OriginalPayoff(IAgent::State st)
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
