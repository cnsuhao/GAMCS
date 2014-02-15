/*
 * member.h
 *
 *  Created on: Feb 11, 2014
 *      Author: andy
 */
#ifndef MEMBER_H
#define MEMBER_H
#include <string>
#include "CSThreadIncarnation.h"

class Member: public CSThreadIncarnation
{
    public:
        Member(std::string);
        ~Member();

    private:
        IAgent::State position;
        int count;

        IAgent::State GetCurrentState();
        void PerformAction(IAgent::Action);
        OSpace ActionCandidates(IAgent::State);
        float OriginalPayoff(IAgent::State);
};

#endif
