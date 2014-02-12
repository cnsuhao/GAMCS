/*
 * Prisoners.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: andy
 */
#include <string>
#include <unistd.h>
#include <stdio.h>
#include "CSThreadAvatar.h"

/**
 * state representation:
 *  |   B\A   | silence | betrayal |
 *  | silence |    1    |    2     |
 *  | betrayal|    3    |    4     |
 *
 * action representation:
 * A:
 *   in silence:    silence(0) , betrayal(1)
 *   in betrayal:   silence(-1), betrayal(0)
 * B:
 *   in silence:    silence(0) , betrayal(2)
 *   in betrayal:   silence(-2), betrayal(0)
 *
 * payoff matrix (payoffA, payoffB):
 *  |   B\A   | silence | betrayal |
 *  | silence | -1, -1  |  0, -10  |
 *  | betrayal| -10, 0  |  -8, -8  |
 */

Agent::State current_state = 1; // start from state 1

/* take turns to act */
bool actA = false;
bool actB = true;    // let A act first

int runtimes = 100;    // number of rounds
bool quit_notification = false; // notify opponent to quit

class PrisonerA: public CSThreadAvatar
{
    public:
        PrisonerA(std::string n) :
                CSThreadAvatar(n)
        {
        }
        ~PrisonerA()
        {
        }

    private:
        Agent::State GetCurrentState()
        {
            while (actB == false)
                usleep(10000);    // sleep, wait for B to act
            actB = false;    // next turn, set actB as false
            return current_state;
        }

        void PerformAction(Agent::Action act)
        {
            current_state += act;
            actA = true;
        }

        OutList ActionCandidates(Agent::State st)
        {
            static int count = 0;
            OutList re;
            re.clear();
            if (count < runtimes)    // run times
            {
                switch (st)
                {
                    case 1:
                        re.add(0);
                        re.add(1);
                        break;
                    case 2:
                        re.add(0);
                        re.add(-1);
                        break;
                    case 3:
                        re.add(0);
                        re.add(1);
                        break;
                    case 4:
                        re.add(0);
                        re.add(-1);
                        break;
                    default:
                        printf(
                                "PrisonerA: ActionCandidates() - unknown state!\n");
                }
                count++;
            }
            else
            {
                actA = true;        //
                quit_notification = true;    // tell B to quit
            }
            return re;
        }

        float OriginalPayoff(Agent::State st)
        {
            float payoff = 0.0;
            switch (st)
            {
                case 1:
                    payoff = -1;
                    break;
                case 2:
                    payoff = 0;
                    break;
                case 3:
                    payoff = -10;
                    break;
                case 4:
                    payoff = -8;
                    break;
                default:
                    printf("PrisonerA: OriginalPayoff() - unknown state!\n");
            }
            return payoff;
        }
};

class PrisonerB: public CSThreadAvatar
{
    public:
        PrisonerB(std::string n) :
                CSThreadAvatar(n)
        {
        }
        ~PrisonerB()
        {
        }

    private:
        Agent::State GetCurrentState()
        {
            while (actA == false)
                usleep(10000);    // sleep, wait for A to act
            actA = false;
            return current_state;
        }

        void PerformAction(Agent::Action act)
        {
            current_state += act;
            actB = true;
        }

        OutList ActionCandidates(Agent::State st)
        {
            OutList re;
            re.clear();

            if (!quit_notification)    // check quit
            {
                switch (st)
                {
                    case 1:
                        re.add(0);
                        re.add(2);
                        break;
                    case 2:
                        re.add(0);
                        re.add(2);
                        break;
                    case 3:
                        re.add(0);
                        re.add(-2);
                        break;
                    case 4:
                        re.add(0);
                        re.add(-2);
                        break;
                    default:
                        printf(
                                "PrisonerA: ActionCandidates() - unknown state!\n");
                }
            }
            return re;
        }

        float OriginalPayoff(Agent::State st)
        {
            float payoff = 0.0;
            switch (st)
            {
                case 1:
                    payoff = -1;
                    break;
                case 2:
                    payoff = -10;
                    break;
                case 3:
                    payoff = 0;
                    break;
                case 4:
                    payoff = -8;
                    break;
                default:
                    printf("PrisonerA: OriginalPayoff() - unknown state!\n");
            }
            return payoff;
        }
};

