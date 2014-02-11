/*
 * member.h
 *
 *  Created on: Feb 11, 2014
 *      Author: andy
 */
#ifndef MEMBER_H
#define MEMBER_H

#include <vector>
#include <string>
#include "CSThreadAvatar.h"

class Member: public CSThreadAvatar
{
    public:
        Member(std::string);
        ~Member();

    private:
        Agent::State position;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        std::vector<Agent::Action> ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif
