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

MemoryViewer::MemoryViewer() :
        storage(NULL)
{
}

MemoryViewer::MemoryViewer(Storage *sg) : storage(sg)
{
    int re = storage->Connect();
    if (re != 0)    // connect failed
        ERROR("MemoryViewer: connect to storage failed!\n");
}

MemoryViewer::~MemoryViewer()
{
    if (storage != NULL)
        storage->Close();
}

void MemoryViewer::SetStorage(Storage *sg)
{
    storage = sg;
    int re = storage->Connect();
    if (re != 0)    // connect failed
        ERROR("MemoryViewer: connect to storage failed!\n");
}
