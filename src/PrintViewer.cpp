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

#include <stdio.h>
#include "gamcs/PrintViewer.h"
#include "gamcs/Storage.h"
#include "gamcs/StateInfoParser.h"

namespace gamcs
{

/**
 * @brief The default constructor.
 *
 * @param [in] sg the storage to be viewed
 */
PrintViewer::PrintViewer(Storage *sg) :
		MemoryViewer(sg)
{
}

/**
 * @brief The default destructor.
 */
PrintViewer::~PrintViewer()
{
}

/**
 * @brief View the whole memory in pretty print style.
 *
 * @param [in] file where to output the view, NULL for standard output
 */
void PrintViewer::view(const char *file)
{
	int re = storage->open(Storage::O_READ);
	if (re != 0)    // connect failed
	{
		WARNNING("PrintViewer Show(): open storage failed!\n");
		return;
	}

	FILE *output = NULL;
	if (file == NULL)    // output to standard output
		output = stdout;
	else
		// output to the requested file
		output = fopen(file, "w");

	// print memory info
	struct Memory_Info *memif = storage->getMemoryInfo();
	if (memif != NULL)
	{
		fprintf(output, "\n");
		fprintf(output,
				"=================== Memory Information ====================\n");
		fprintf(output, "discount rate: \t%.2f\n", memif->discount_rate);
		fprintf(output, "threshold: \t%.2f\n", memif->threshold);
		fprintf(output, "number of states: \t%" UINT32_FMT "\n", memif->state_num);
		fprintf(output, "number of links: \t%" UINT32_FMT "\n", memif->lk_num);
		fprintf(output, "last state: \t%" ST_FMT "\n", memif->last_st);
		fprintf(output, "last action: \t%" ACT_FMT "\n", memif->last_act);
		free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
		fprintf(output,
				"===========================================================\n\n");
	}
	else
	{
		fprintf(output, "Memory not found in storage!\n");
		storage->close();
		return;
	}

	// print states info
	Agent::State st = storage->firstState();
	while (st != Agent::INVALID_STATE)    // get state value
	{
		struct State_Info_Header *stif = storage->getStateInfo(st);
		if (stif != NULL)
		{
			printStateInfo(stif, output);
			free(stif);
			st = storage->nextState();
		}
		else
			ERROR("Show(): state: %" ST_FMT " information is NULL!\n", st);
	}
	storage->close();
}

/**
 * @brief View a state information in pretty print style.
 *
 * @param [in] sthd the state information
 * @param [in] output stream to output the view, NULL for standard output
 */
void PrintViewer::printStateInfo(const struct State_Info_Header *sthd,
		FILE *output) const
{
	if (sthd == NULL)
		return;

	fprintf(output, "++++++++++++++++++++++++ State: %" ST_FMT " ++++++++++++++++++++++++++\n",
			sthd->st);
	fprintf(output,
			"Original payoff: %.2f,\t Payoff: %.2f,\t Count: %" UINT32_FMT ", ActNum: %" UINT32_FMT "\n",
			sthd->original_payoff, sthd->payoff, sthd->count, sthd->act_num);

	fprintf(output,
			"------------------------------------------------------------\n");

	StateInfoParser sparser(sthd);
	Action_Info_Header *athd = NULL;
	EnvAction_Info *eaif = NULL;

	athd = sparser.firstAct();
	while (athd != NULL)
	{
		eaif = sparser.firstEat();
		while (eaif != NULL)
		{
			fprintf(output, "\t  .|+++ %" ACT_FMT " +++ %" ACT_FMT " ++> %" ST_FMT " \t Count: %" UINT32_FMT "\n", athd->act,
					eaif->eat, eaif->nst, eaif->count);

			eaif = sparser.nextEat();
		}

		athd = sparser.nextAct();
	}
	fprintf(output,
			"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

	return;
}

/**
 * @brief View a specified state in graphviz dot style.
 *
 * @param [in] st the state to be viewed
 * @param [in] file where to output the view, NULL for standard output
 */
void PrintViewer::viewState(Agent::State st, const char *file)
{
	int re = storage->open(Storage::O_READ);
	if (re != 0)    // connect failed
	{
		WARNNING("PrintViewer ShowState(): open storage failed!\n");
		return;
	}

	FILE *output = NULL;
	if (file == NULL)    // output to standard output
		output = stdout;
	else
		// output to the requested file
		output = fopen(file, "w");

	struct State_Info_Header *stif = storage->getStateInfo(st);
	if (stif != NULL)
	{
		printStateInfo(stif, output);
		free(stif);
	}
	else
	{
	fprintf(output, "state %" ST_FMT " not found in memory!\n", st);
}
storage->close();
}

}    // namespace gamcs
