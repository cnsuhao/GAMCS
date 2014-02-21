// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <set>
#include "gimcs/ExNet.h"

namespace gimcs
{

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

bool ExNet::HasMember(int mid) const
{
    bool re = false;
    std::set<int> allmembers = GetAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

}    // namespace gimcs
