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
#include "ExNetwork.h"


ExNetwork::ExNetwork() :
        id(0)
{
}

ExNetwork::ExNetwork(int i) :
        id(i)
{
}

ExNetwork::~ExNetwork()
{
}

bool ExNetwork::hasMember(int mid) const
{
    bool re = false;
    std::set<int> allmembers = getAllMembers();
    if (allmembers.find(mid) != allmembers.end())    // found
    re = true;

    return re;
}

