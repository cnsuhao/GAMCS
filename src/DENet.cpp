/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <set>
#include "DENet.h"
#include "Debug.h"

DENet::DENet() :
        id(0)
{
}

DENet::DENet(int i) :
        id(i)
{
}

DENet::~DENet()
{
}

bool DENet::HasMember(int mid)
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

