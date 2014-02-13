/**********************************************************************
 *	@File:
 *	@Created: 2013
 *	@Author: AndyHuang - andyspider@126.coom
 *	@Description:
 *
 *	@Modify date:
 ***********************************************************************/
#include <set>
#include "ParallelNet.h"
#include "Debug.h"

ParallelNet::ParallelNet() :
        id(0)
{
}

ParallelNet::ParallelNet(int i) :
        id(i)
{
}

ParallelNet::~ParallelNet()
{
}

bool ParallelNet::HasMember(int mid)
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

