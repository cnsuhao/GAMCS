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

#ifndef DOTVIEWER_H_
#define DOTVIEWER_H_
#include <string>
#include "gamcs/MemoryViewer.h"
#include "gamcs/Agent.h"

namespace gamcs
{

class Storage;

/**
 * View memory in graphviz dot style.
 */
class DotViewer: public MemoryViewer
{
	public:
		DotViewer(Storage *storage = NULL);
		virtual ~DotViewer();

		void view(const char *file = NULL);
		void viewState(Agent::State state, const char *file = NULL);

	protected:
		const std::string int2String(gamcs_int value) const;

	private:
		void dotStateInfo(
				const struct State_Info_Header *state_information_header,
				FILE *output) const;

		Agent::State last_state; /**< the last state when memory was dumped */
		Agent::Action last_action; /**< the last action when memory was dumped */
};

}    // namespace gamcs
#endif /* DOTVIEWER_H_ */
