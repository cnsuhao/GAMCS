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
// Created on: Feb 21, 2014
//
// -----------------------------------------------------------------------------

#ifndef CDOTVIEWER_H_
#define CDOTVIEWER_H_

#include "gamcs/DotViewer.h"
#include "gamcs/MemoryViewer.h"
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 * View memory in graphviz dot style cleanly.
 */
class CDotViewer: public DotViewer
{
	public:
		CDotViewer(Storage *storage = NULL);
		virtual ~CDotViewer();

		void view(const char *file = NULL);

	private:
		void cleanDotStateInfo(
				const struct State_Info_Header * state_information_header,
				FILE *output) const;

		Agent::State last_state; /**< the last state when memory was dumped */
		Agent::Action last_action; /**< the last action when memory was dumped */
};

} /* namespace gamcs */

#endif /* CDOTVIEWER_H_ */
