/*
 * ExNet.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */
#include <set>
#include "ExNet.h"

ExNet::ExNet() :
        id(0)
{
}

ExNet::ExNet(int i) :
        id(i)
{
}

ExNet::~ExNet()
{
}

bool ExNet::HasMember(int mid)
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}
