// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#ifndef SALEMAN_H_
#define SALEMAN_H_
#include <math.h>
#include "gamcs/Avatar.h"

using namespace gamcs;

const int CITY_NUM = 8;    // number of cities, start from 0

/***************************************************************************/

/* Distance between cities：                                               */

/*           北京  天津  武汉   深圳   长沙   成都   杭州  西安  拉萨   南昌    */

/*           (0)   (1)   (2)   (3)   (4)   (5)   (6)   (7)   (8)   (9)     */

/*  北京(0)   0     1    1272  2567  1653  2097  1425  1177  3947   1       */

/*  天津(1)   1     0     1    2511  1633  2077  1369  1157  3961  1518     */

/*  武汉(2)  1272   1     0     1    380   1490  821   856   3660  385      */

/*  深圳(3)  2567  2511   1     0     1    2335  1562  2165  3995  933      */

/*  长沙(4)  1653  1633  380    1     0     1    1041  1135  3870  456      */

/*  成都(5)  2097  2077  1490  2335   1     0     1    920   2170  1920     */

/*  杭州(6)  1425  1369  821   1562  1041   1     0     1    4290  626      */

/*  西安(7)  1177  1157  856   2165  1135  920    1     0     1    1290     */

/*  拉萨(8)  3947  3961  3660  3995  3870  2170  4290   1     0     1       */

/*  南昌(9)   1    1518  385   993   456   1920  626   1290   1     0       */

/****************************************************************************/

int distance_table[10][10] = {
        { 0, 1, 1272, 2567, 1653, 2097, 1425, 1, 3947, 1 }, { 1, 0, 1, 2511,
                1633, 2077, 1369, 1157, 3961, 1518 }, { 1272, 1, 0, 1, 380,
                1490, 821, 856, 3660, 385 }, { 2567, 2511, 1, 0, 1, 2335, 1562,
                2165, 3995, 933 }, { 1653, 1633, 380, 1, 0, 1, 1041, 1135, 3870,
                456 }, { 2097, 2077, 1490, 2335, 1, 0, 1, 920, 2170, 1920 }, {
                1425, 1369, 821, 1562, 1041, 1, 0, 1, 4290, 626 }, { 1177, 1157,
                856, 2165, 1135, 920, 1, 0, 1, 1290 }, { 3947, 3961, 3660, 3995,
                3870, 2170, 4290, 1, 0, 1 }, { 1, 1518, 385, 993, 456, 1920,
                626, 1290, 1, 0 } };

/**
 *
 */
class Saleman: public Avatar
{
    public:
        Saleman() :
                current_state(Agent::INVALID_STATE)
        {
            for (int i = 0; i < CITY_NUM; i++)
            {
                path[i] = i;
            }
        }
        virtual ~Saleman()
        {
        }

    private:
        Agent::State perceiveState()
        {
            // encode state
            encodeState(path, current_state);
            printf("Saleman State: <%" ST_FMT ">,\t", current_state);
            printPath(path);

            return current_state;
        }

        OSpace availableActions(Agent::State st)
        {
            OSpace re;

            int tmp_path[CITY_NUM];
            decodeState(st, tmp_path);

            int acts[CITY_NUM];
            Agent::Action act;

            // actions that swap any two cities
            for (int delta = 1; delta < CITY_NUM; delta++)
            {
                for (int i = 0; i < CITY_NUM; i++)
                {
                    for (int k = 0; k < CITY_NUM; k++)    // clear
                        acts[k] = 0;

                    for (int j = 0; j < CITY_NUM; j++)
                    {
                        if (tmp_path[j] - tmp_path[i] == delta)
                        {
                            acts[i] = delta;
                            acts[j] = -delta;
                            encodeAction(acts, act);
                            re.add(act);
                            break;
                        }
                    }
                }
            }

            return re;
        }

        void performAction(Agent::Action act)
        {
            // apply act
            current_state += act;

            decodeState(current_state, path);
        }

        float originalPayoff(Agent::State st)
        {
            // decode state
            int tmp_path[CITY_NUM];
            decodeState(st, tmp_path);
            // calculate the path length
            int city, ncity;
            int total_distance = 0;
            for (int i = 0; i < CITY_NUM - 1; i++)
            {
                city = tmp_path[i];
                ncity = tmp_path[i + 1];

                total_distance += distance_table[city][ncity];
            }
            // loop end to start
            city = tmp_path[CITY_NUM - 1];
            ncity = tmp_path[0];
            total_distance += distance_table[city][ncity];

            printf("total_distance: %d\n", total_distance);
            return -total_distance;

        }

        void encodeState(const int path[CITY_NUM], Agent::State &encoded_st)
        {
            encoded_st = 0;
            for (int i = 0; i < CITY_NUM; i++)
            {
                encoded_st += path[i] * pow((float) CITY_NUM, (CITY_NUM - 1 - i));    // path[CN-1]*CN^0 + path[CN-2]*CN^1 + ... + path[1]*CN^(CN-2) + path[0]*CN^(CN-1)
            }
        }

        void decodeState(Agent::State st, int decode_path[CITY_NUM])
        {
            for (int i = CITY_NUM - 1; i >= 0; i--)
            {
                unsigned long quotient = st / (CITY_NUM);
                int remainder = st % (CITY_NUM);

                // the ist city
                decode_path[i] = remainder;

                st = quotient;
            }
        }

        void encodeAction(const int act[CITY_NUM], Agent::Action &encoded_act)
        {
            encoded_act = 0;
            for (int i = 0; i < CITY_NUM; i++)
            {
                encoded_act += act[i] * pow((float) CITY_NUM, (CITY_NUM - 1 - i));
            }
        }

        void printPath(int path[CITY_NUM])
        {
            printf("Path: ");
            for (int i = 0; i < CITY_NUM; i++)
            {
                printf("%d --> ", path[i]);
            }
            printf("%d\n", path[0]);    // loop
        }
    private:
        int path[CITY_NUM];    // represent a path through all cities
        int count;
        Agent::State current_state;    // current state
};

#endif /* SALEMAN_H_ */
