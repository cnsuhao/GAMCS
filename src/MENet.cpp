/*
 * MENet.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */
#include <set>
#include "MENet.h"

MENet::MENet() :
        id(0)
{
}

MENet::MENet(int i) :
        id(i)
{
}

MENet::~MENet()
{
}

bool MENet::HasMember(int mid)
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}
