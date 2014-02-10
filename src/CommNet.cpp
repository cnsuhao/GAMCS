/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <set>
#include "CommNet.h"
#include "Debug.h"

CommNet::CommNet() :
        id(0)
{
}

CommNet::CommNet(int i) :
        id(i)
{
}

CommNet::~CommNet()
{
}

bool CommNet::HasMember(int mid)
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

