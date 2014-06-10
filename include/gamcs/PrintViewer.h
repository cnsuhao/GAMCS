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

#ifndef PRINTVEWER_H_
#define PRINTVEWER_H_
#include "gamcs/MemoryViewer.h"
#include "gamcs/Agent.h"

namespace gamcs
{

class Storage;

/**
 * View memory in pretty print style.
 */
class PrintViewer: public MemoryViewer
{
	public:
		PrintViewer(Storage *storage = NULL);
		virtual ~PrintViewer();

		void view(const char *file = NULL);
		void viewState(Agent::State state, const char *file = NULL);

	private:
		void printStateInfo(
				const struct State_Info_Header *state_information_header,
				FILE *output) const;
};

}    // namespace gamcs
#endif /* PRINTVEWER_H_ */
