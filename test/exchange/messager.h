/*
 * messager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef MESSAGER_H_
#define MESSAGER_H_
#include "CSThreadExManager.h"

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

        Agent::State Incar_GetCurrentState()
        {
            printf("Messager %d, State: %ld\n", id, position);
            return position;
        }

        void Incar_PerformAction(Agent::Action act)
        {
            position += act;

            if (position > 15) position = 15;
            if (position < 1) position = 1;
            return;
        }

        OSpace Incar_ActionCandidates(Agent::State st)
        {
            if (count < 1000)
            {
                OSpace acts;
                acts.Clear();
                if (st == 1)    // position 1
                {
                    acts.Add(1);
                    count++;
                    return acts;
                }

                acts.Add(1);
                acts.Add(-1);
                count++;
                return acts;
            }
            else
                return OSpace();    // return an empty list
        }

        float Incar_OriginalPayoff(Agent::State st)
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
