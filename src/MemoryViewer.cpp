/*
 * MemoryViewer.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: andy
 */

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
