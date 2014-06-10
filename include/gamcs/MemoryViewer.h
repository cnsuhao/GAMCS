// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// Created on: Feb 7, 2014
//
// -----------------------------------------------------------------------------

#ifndef MEMORYVIEWER_H_
#define MEMORYVIEWER_H_
#include "gamcs/debug.h"
#include "gamcs/Agent.h"

namespace gamcs
{

class Storage;

/**
 * Interface for visualizing memory.
 */
class MemoryViewer
{
	public:
		/**
		 * @brief The default constructor.
		 *
		 * @param [in] sg the storage to attach to
		 */
		MemoryViewer(Storage *sg = NULL) :
				storage(sg)
		{
		}

		/**
		 * @brief The default destructor.
		 */
		virtual ~MemoryViewer()
		{
		}

		void attachStorage(Storage *specific_storage);

		/**
		 * @brief View the whole memory.
		 *
		 * @param [in] file where to output the view
		 */
		virtual void view(const char *file = NULL) = 0;

		/**
		 * @brief View a specified state.
		 *
		 * @param [in] state the state to view
		 * @param [in] file where to output the view
		 */
		virtual void viewState(Agent::State state, const char *file = NULL) = 0;

	protected:
		Storage *storage; /**< the storage where memory is stored */
};

/**
 * @brief Attach memory viewer to a storage, the viewer will show the memory stored in it.
 *
 * @param [in] sg the storage to be visualized
 */
inline void MemoryViewer::attachStorage(Storage *sg)
{
	storage = sg;
}

}    // namespace gamcs
#endif /* MEMORYVIEWER_H_ */
