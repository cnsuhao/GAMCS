// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------


#include <set>
#include "gamcs/ExNet.h"

namespace gamcs
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

bool ExNet::hasMember(int mid) const
{
    bool re = false;
    std::set<int> allmembers = getAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

}    // namespace gamcs
