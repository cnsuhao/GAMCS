/*
 * saleman.h
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#ifndef SALEMAN_H_
#define SALEMAN_H_
#include <string>
#include <CSThreadAvatar.h>

const int CITY_NUM = 10;    // number of cities
int path[CITY_NUM];    // represent a path through all cities
Agent::State current_state = INVALID_STATE;    // current state

/*
 *
 */
class Saleman: public CSThreadAvatar
{
    public:
        Saleman(std::string n) :
                CSThreadAvatar(n), count(0)
        {
        }
        virtual ~Saleman()
        {
        }

    private:
        int count;

        Agent::State GetCurrentState()
        {
            // encode state
            for (int i = 0; i < CITY_NUM; i++)
            {
                current_state += path[i] * CITY_NUM ^ (CITY_NUM - 1 - i);    // path[CN-1]*CN^0 + path[CN-2]*CN^1 + ... + path[1]*CN^(CN-2) + path[0]*CN^(CN-1)
            }

            return current_state;
        }

        std::vector<Agent::Action> ActionCandidates(Agent::State st)
        {
            // encode action
            std::vector<Agent::Action> acts;
            acts.clear();

        }

        void PerformAction(Agent::Action act)
        {
            // decode action
            Agent::Action tmp_act = act;
            // [CITY_NUM,0], from low order to high order
            for (int i = CITY_NUM - 1; i >= 0; i--)
            {
                long quotient = tmp_act / (2 * CITY_NUM);
                int remainder = tmp_act % (2 * CITY_NUM);
                path[i] += remainder;

                tmp_act = quotient;
            }
        }

        float OriginalPayoff(Agent::State st)
        {
            // decode state
            int tmp_path[CITY_NUM];

            for (int i = CITY_NUM - 1; i >= 0; i--)
            {
                unsigned long quotient = st / (CITY_NUM);
                int remainder = st % (CITY_NUM);

                // the ist city
                tmp_path[i] = remainder;

                st = quotient;
            }
            // calculate the path length

        }
};

#endif /* SALEMAN_H_ */
