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
#include <math.h>

const int CITY_NUM = 10;    // number of cities
int path[CITY_NUM];    // represent a path through all cities
Agent::State current_state = INVALID_STATE;    // current state

/***************************************************************************/

/* 城市地图的距离信息：                                                       */

/*           北京  天津   武汉   深圳   长沙   成都   杭州  西安  拉萨   南昌    */

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

int distance_table[10][10] = { { 0, 1, 1272, 2567, 1653, 2097, 1425, 1177, 3947,
        1 },

{ 1, 0, 1, 2511, 1633, 2077, 1369, 1157, 3961, 1518 },

{ 1272, 1, 0, 1, 380, 1490, 821, 856, 3660, 385 },

{ 2567, 2511, 1, 0, 1, 2335, 1562, 2165, 3995, 933 },

{ 1653, 1633, 380, 1, 0, 1, 1041, 1135, 3870, 456 },

{ 2097, 2077, 1490, 2335, 1, 0, 1, 920, 2170, 1920 },

{ 1425, 1369, 821, 1562, 1041, 1, 0, 1, 4290, 626 },

{ 1177, 1157, 856, 2165, 1135, 920, 1, 0, 1, 1290 },

{ 3947, 3961, 3660, 3995, 3870, 2170, 4290, 1, 0, 1 },

{ 1, 1518, 385, 993, 456, 1920, 626, 1290, 1, 0 } };

/**
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
            EncodeState(path, current_state);

            return current_state;
        }

        OutList ActionCandidates(Agent::State st)
        {
            int tmp_path[CITY_NUM];
            DecodeState(st, tmp_path);

            int min_acts[CITY_NUM];
            int max_acts[CITY_NUM];
            for (int i = 0; i < CITY_NUM; i++)
            {
                min_acts[i] = 1 - tmp_path[i];
                max_acts[i] = CITY_NUM - tmp_path[i];
            }
            // encode action
            Agent::Action start, end;
            EncodeAction(min_acts, start);
            EncodeAction(max_acts, end);

            OutList acts;
            acts.add(start, end, 1);
            return acts;
        }

        void PerformAction(Agent::Action act)
        {
            // decode action
            int acts[CITY_NUM];
            DecodeAction(act, acts);

            for (int i = 0; i < CITY_NUM; i++)
            {
                path[i] += acts[i];
            }
        }

        float OriginalPayoff(Agent::State st)
        {
            // decode state
            int tmp_path[CITY_NUM];
            DecodeState(st, tmp_path);
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
            city = tmp_path[CITY_NUM];
            ncity = tmp_path[0];
            total_distance += distance_table[city][ncity];

            return 99999999 - total_distance;

        }

    public:
        void EncodeState(const int path[CITY_NUM], Agent::State &encoded_st)
        {
            encoded_st = 0;
            for (int i = 0; i < CITY_NUM; i++)
            {
                encoded_st += path[i] * pow(CITY_NUM, (CITY_NUM - 1 - i));    // path[CN-1]*CN^0 + path[CN-2]*CN^1 + ... + path[1]*CN^(CN-2) + path[0]*CN^(CN-1)
            }
        }

        void DecodeState(Agent::State st, int decode_path[CITY_NUM])
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

        void EncodeAction(const int act[CITY_NUM], Agent::Action &encoded_act)
        {
            encoded_act = 0;
            for (int i = 0; i < CITY_NUM; i++)
            {
                encoded_act += act[i] * pow(CITY_NUM, (CITY_NUM - 1 - i));
            }
        }

        void DecodeAction(Agent::Action act, int decode_acts[CITY_NUM])
        {
            // [CITY_NUM,0], from low order to high order
            for (int i = CITY_NUM - 1; i >= 0; i--)
            {
                long quotient = act / (CITY_NUM);
                int remainder = act % (CITY_NUM);
                decode_acts[i] = remainder;

                act = quotient;
            }
        }
};

#endif /* SALEMAN_H_ */
