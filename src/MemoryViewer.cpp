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


#include <stddef.h>
#include "MemoryViewer.h"
#include "Storage.h"
#include "Debug.h"

namespace gimcs
{

MemoryViewer::MemoryViewer() :
        storage(NULL)
{
}

MemoryViewer::MemoryViewer(Storage *sg) :
        storage(sg)
{
}

MemoryViewer::~MemoryViewer()
{
}

}    // namespace gimcs
