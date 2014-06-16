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

#include <string>
#include <vector>
#include <stdio.h>
#include "gamcs/DotViewer.h"
#include "gamcs/Agent.h"
#include "gamcs/Storage.h"
#include "gamcs/StateInfoParser.h"

namespace gamcs
{

/**
 * @brief The default constructor.
 *
 * @param [in] sg the storage to be viewed
 */
DotViewer::DotViewer(Storage *sg) :
		MemoryViewer(sg), last_state(Agent::INVALID_STATE), last_action(
				Agent::INVALID_ACTION)
{
}

/**
 * @brief The default destructor.
 */
DotViewer::~DotViewer()
{
}

/**
 * @brief View the whole memory in graphviz dot style.
 *
 * @param [in] file where to output the view, NULL for standard output
 * @dot
 * digraph Example
 * {
 * label="memory Example\ndiscount rate: 0.90, accuracy: 0.01, #states: 7, #links: 11"
 * node [color=black,shape=circle]
 * rank="same"
 *
 * subgraph state8
 * {
 * rank="same"
 * st8 [label="8\n(3.79)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act1in8 [label="", height=0.3]
 * }
 * st8 -> act1in8 [label=<<font color="blue">1</font>>, color="blue", weight=2.]
 * }
 * act1in8 -> st7 [label=<<font color="red">-2 (2)</font>>, color="red", weight=1.]
 *
 * subgraph state7
 * {
 * rank="same"
 * st7 [label="7\n(4.21)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act1in7 [label="", height=0.3]
 * act2in7 [label="", height=0.3]
 * act1in7 -> act2in7 [style=dashed, dir=none]
 * }
 * st7 -> act1in7 [label=<<font color="blue">1</font>>, color="blue", weight=2.]
 * st7 -> act2in7 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * }
 * act1in7 -> st6 [label=<<font color="red">-2 (1)</font>>, color="red", weight=1.]
 * act2in7 -> st8 [label=<<font color="red">-1 (2)</font>>, color="red", weight=1.]
 *
 * subgraph state6
 * {
 * rank="same"
 * st6 [label="6\n(4.69)", color="#D3D300"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act1in6 [label="", height=0.3]
 * act2in6 [label="", height=0.3]
 * act1in6 -> act2in6 [style=dashed, dir=none]
 * }
 * st6 -> act1in6 [label=<<font color="#D3D300">1</font>>, color="#D3D300", weight=2.]
 * st6 -> act2in6 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * }
 * act1in6 -> st5 [label=<<font color="red">-2 (242)</font>>, color="red", weight=1.]
 * act2in6 -> st7 [label=<<font color="red">-1 (1)</font>>, color="red", weight=1.]
 *
 * subgraph state5
 * {
 * rank="same"
 * st5 [label="5\n(5.21)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act2in5 [label="", height=0.3]
 * }
 * st5 -> act2in5 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * }
 * act2in5 -> st6 [label=<<font color="red">-1 (243)</font>>, color="red", weight=1.]
 *
 * subgraph state4
 * {
 * rank="same"
 * st4 [label="4\n(4.69)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act2in4 [label="", height=0.3]
 * act1in4 [label="", height=0.3]
 * act2in4 -> act1in4 [style=dashed, dir=none]
 * }
 * st4 -> act2in4 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * st4 -> act1in4 [label=<<font color="blue">1</font>>, color="blue", weight=2.]
 * }
 * act2in4 -> st5 [label=<<font color="red">-1 (1)</font>>, color="red", weight=1.]
 * act1in4 -> st3 [label=<<font color="red">-2 (1)</font>>, color="red", weight=1.]
 *
 * subgraph state2
 * {
 * rank="same"
 * st2 [label="2\n(3.79)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act2in2 [label="", height=0.3]
 * }
 * st2 -> act2in2 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * }
 * act2in2 -> st3 [label=<<font color="red">-1 (2)</font>>, color="red", weight=1.]
 *
 * subgraph state3
 * {
 * rank="same"
 * st3 [label="3\n(4.21)", color="black"]
 * subgraph
 * {
 * rank="sink"
 * node [shape="point"]
 * act2in3 [label="", height=0.3]
 * act1in3 [label="", height=0.3]
 * act2in3 -> act1in3 [style=dashed, dir=none]
 * }
 * st3 -> act2in3 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * st3 -> act1in3 [label=<<font color="blue">1</font>>, color="blue", weight=2.]
 * }
 * act2in3 -> st4 [label=<<font color="red">-1 (2)</font>>, color="red", weight=1.]
 * act1in3 -> st2 [label=<<font color="red">-2 (2)</font>>, color="red", weight=1.]
 * }
 * @enddot
 */
void DotViewer::view(const char *file)
{
	int re = storage->open(Storage::O_READ);
	if (re != 0)    // connect failed
	{
		WARNNING("DotViewer Show(): open storage failed!\n");
		return;
	}

	FILE *output = NULL;
	if (file == NULL)    // output to standard ouput
		output = stdout;
	else
		output = fopen(file, "w");

	fprintf(output,
			"/* This is the dot file of agent memory automaticlly generated by DotViewer */\n\n");

	// generate dot syntax
	fprintf(output, "digraph %s \n{\n", storage->getMemoryName().c_str());

	// memory info
	struct Memory_Info *memif = storage->getMemoryInfo();
	if (memif != NULL)
	{
		fprintf(output,
				"label=\"memory %s\\ndiscount rate: %.2f, accuracy: %.2f, #states: %" UINT32_FMT ", #links: %" UINT32_FMT "\"\n",
				storage->getMemoryName().c_str(), memif->discount_rate,
				memif->accuracy, memif->state_num, memif->lk_num);
		// store last status
		last_state = memif->last_st;
		last_action = memif->last_act;
		free(memif);    // free it, the memory struct are not a substaintial struct for running, it's just used to store meta-memory information
	}
	else
	{
		fprintf(output, "Memory not found in storage!\n");
		fprintf(output, "}\n");    // digraph
		storage->close();
		return;
	}

	// states info
	fprintf(output, "node [color=black,shape=circle]\n");
	fprintf(output, "rank=\"same\"\n");
	// print states info
	Agent::State st = storage->firstState();
	while (st != Agent::INVALID_STATE)    // get state value
	{
		struct State_Info_Header *stif = storage->getStateInfo(st);
		if (stif != NULL)
		{
			dotStateInfo(stif, output);
			free(stif);
			st = storage->nextState();
		}
		else
			ERROR("Show(): state: %" ST_FMT " information is NULL!\n", st);
	}
	fprintf(output, "}\n");    // digraph
	storage->close();
}

/**
 * @brief View a state information in dot style.
 *
 * @param [in] sthd the state information
 * @param [in] output stream to output the view, NULL for standard output
 */
void DotViewer::dotStateInfo(const struct State_Info_Header *sthd,
		FILE *output) const
{
	/* generated state example:
	 *
	 * subgraph state9
	 * {
	 *      rank="same"
	 *      st9 [label="9"]
	 *      subgraph
	 *      {
	 *          rank="sink"
	 *          node [shape="point"]
	 *          act0in9 [label=""]
	 *          act1in9 [label=""]
	 *          act0in9 -> act1in9 [style=dashed, dir=none]
	 *      }
	 *      st9 -> act0in9 [label="act: -1"]
	 *      st9 -> act1in9 [label="act: 1"]
	 * }
	 *
	 * act0in9 -> st8 [label="eat: 0 (count)"]
	 * act1in9 -> st7 [label="eat: 0 (count)"]
	 */

	if (sthd == NULL)
		return;

	std::string st_color;
	if (sthd->st == last_state)
		st_color = "#D3D300";    // highlight last state node
	else
		st_color = "black";

	std::vector<Agent::Action> acts(sthd->act_num);

	fprintf(output, "\nsubgraph state%s \n{\n", int2String(sthd->st).c_str());
	fprintf(output, "rank=\"same\"\n");
	fprintf(output, "st%s [label=\"%" ST_FMT "\\n(%.2f)\", color=\"%s\"]\n", int2String(sthd->st).c_str(), sthd->st,
			sthd->payoff, st_color.c_str());
	// action nodes and action ---> action
	fprintf(output, "subgraph \n{\n");
	fprintf(output, "rank=\"sink\"\n");    // env nodes should be drawing under state node
	fprintf(output, "node [shape=\"point\"]\n");

	Action_Info_Header *achd, *pre_achd = NULL;
	StateInfoParser sparser(sthd);
	achd = sparser.firstAct();
	int i = 0;
	while (achd != NULL)
	{
		fprintf(output, "act%sin%s [label=\"\", height=0.3]\n",
				int2String(achd->act).c_str(), int2String(sthd->st).c_str());
		if (pre_achd != NULL)
			fprintf(output, "act%sin%s -> act%sin%s [style=dashed, dir=none]\n",
					int2String(pre_achd->act).c_str(),
					int2String(sthd->st).c_str(), int2String(achd->act).c_str(),
					int2String(sthd->st).c_str());

		acts[i++] = achd->act;    // save for using later
		pre_achd = achd;

		achd = sparser.nextAct();
	}
	fprintf(output, "}\n");    // subgraph

	// state ---> actions
	for (unsigned long i = 0; i < sthd->act_num; i++)
	{
	if (sthd->st == last_state && acts[i] == last_action)    // highlight last action edge
	fprintf(output,
			"st%s -> act%sin%s [label=<<font color=\"#D3D300\">%" ACT_FMT "</font>>, color=\"#D3D300\", weight=2.]\n",
			int2String(sthd->st).c_str(), int2String(acts[i]).c_str(), int2String(sthd->st).c_str(), acts[i]);
	else
	fprintf(output,
			"st%s -> act%sin%s [label=<<font color=\"blue\">%" ACT_FMT "</font>>, color=\"blue\", weight=2.]\n",
			int2String(sthd->st).c_str(), int2String(acts[i]).c_str(), int2String(sthd->st).c_str(), acts[i]);

}

fprintf(output, "}\n");    // end of subgraph state

EnvAction_Info *eaif = NULL;
achd = sparser.firstAct();    // restart from beginning
while (achd != NULL)
{
	eaif = sparser.firstEat();
	while (eaif != NULL)
	{
		fprintf(output,
				"act%sin%s -> st%s [label=<<font color=\"red\">%" ACT_FMT " (%" UINT32_FMT ")</font>>, color=\"red\", weight=1.]\n",
				int2String(achd->act).c_str(), int2String(sthd->st).c_str(), int2String(eaif->nst).c_str(),
				eaif->eat, eaif->count);

		eaif = sparser.nextEat();
	}

	achd = sparser.nextAct();
}
}

/**
 * @brief Convert from a integer value to string.
 *
 * This is due to graphviz dot doesn't support minus symbol in node names
 * @param [in] value the integer value
 * @return the converted string
 */
const std::string DotViewer::int2String(gamcs_int value) const
{
char tmp[28];
if (value >= 0)
{
	sprintf(tmp, "%" GAMCS_INT_FMT, value);
	std::string str(tmp);
	return str;
}
else    // eat < 0, since dot doesn't support minus sign, so we convert '-' to '_'
{
	sprintf(tmp, "_%" GAMCS_INT_FMT, -value);
	std::string str(tmp);
	return str;
}
}

/**
 * @brief View a specified state in graphviz dot style.
 *
 * @param [in] st the state to be viewed
 * @param [in] file where to output the view, NULL for standard output
 * @dot
 * digraph State7
 * {
 * node [color=black,shape=circle]
 * rank="same"
 * label="infoset of state 7 in memory Example"
 * rank="same"
 * st7 [label="7\n(4.21)"]
 * subgraph
 * {
 * rank="same"
 * node [shape="point"]
 * act1in7 [label="", height=0.3]
 * act2in7 [label="", height=0.3]
 * act1in7 -> act2in7 [style=dashed, dir=none]
 * }
 * st7 -> act1in7 [label=<<font color="blue">1</font>>, color="blue", weight=2.]
 * st7 -> act2in7 [label=<<font color="blue">2</font>>, color="blue", weight=2.]
 * act1in7 -> st6 [label=<<font color="red">-2 (1)</font>>, color="red", weight=1.]
 * st6 [label="6\n(4.69)"]
 * act2in7 -> st8 [label=<<font color="red">-1 (2)</font>>, color="red", weight=1.]
 * st8 [label="8\n(3.79)"]
 * }
 * @enddot
 */
void DotViewer::viewState(Agent::State st, const char *file)
{
int re = storage->open(Storage::O_READ);
if (re != 0)    // connect failed
{
	WARNNING("DotViewer ShowState(): open storage failed!\n");
	return;
}

FILE *output = NULL;
if (file == NULL)
	output = stdout;
else
	output = fopen(file, "w");

fprintf(output,
		"/* This is the dot file of agent memory automaticlly generated by DotViewer */\n\n");

// generated dot file example:
/*
 * digraph Mouse_1 {
 * node [color=black,shape=circle]
 * rank="same"
 * st1 [label="1\n(3.00)"]
 *
 * subgraph {
 * rank="same"
 * eat1in1 [shape="point", label="", height=0.3]
 * eat0in1 [shape="point", label="", height=0.3]
 * eat1in1 -> eat0in1 [style=dashed, dir=none]
 * }
 * st1 -> eat1in1 [label=<<font color="blue">e: 1 (1)</font>>, color="blue", weight=3.]
 * st1 -> eat0in1 [label=<<font color="blue">e: 0 (4)</font>>, color="blue", weight=3.]
 *
 * eat1in1 -> st1 [label=<<font color="red">a: -1 (0.60)</font>>, color="red", weight=1.]
 * eat0in1 -> st2 [label=<<font color="red">a: 1 (2.74)</font>>, color="red", weight=1.]
 * st2 [label="2\n(1.00)"]
 * }
 */

fprintf(output, "digraph %s_%s \n{\n", storage->getMemoryName().c_str(),
		int2String(st).c_str());
// state info
fprintf(output, "node [color=black,shape=circle]\n");
fprintf(output, "rank=\"same\"\n");
fprintf(output, "label=\"infoset of state %" ST_FMT " in memory %s\"\n", st,
		storage->getMemoryName().c_str());

struct State_Info_Header *sthd = storage->getStateInfo(st);
if (sthd != NULL)
{
	std::vector<Agent::Action> acts(sthd->act_num);

	fprintf(output, "rank=\"same\"\n");
	fprintf(output, "st%s [label=\"%" ST_FMT "\\n(%.2f)\"]\n", int2String(sthd->st).c_str(), sthd->st,
			sthd->payoff);
	// action nodes and action ---> action
	fprintf(output, "subgraph \n{\n");
	fprintf(output, "rank=\"same\"\n");    // env nodes should be drawing under state node
	fprintf(output, "node [shape=\"point\"]\n");

	StateInfoParser sparser(sthd);
	Action_Info_Header *achd, *pre_achd = NULL;
	achd = sparser.firstAct();
	int i = 0;
	while (achd != NULL)
	{
		fprintf(output, "act%sin%s [label=\"\", height=0.3]\n",
				int2String(achd->act).c_str(), int2String(sthd->st).c_str());
		if (pre_achd != NULL)
			fprintf(output, "act%sin%s -> act%sin%s [style=dashed, dir=none]\n",
					int2String(pre_achd->act).c_str(),
					int2String(sthd->st).c_str(), int2String(achd->act).c_str(),
					int2String(sthd->st).c_str());

		acts[i++] = achd->act;    // save for using later
		pre_achd = achd;
		achd = sparser.nextAct();
	}
	fprintf(output, "}\n");    // subgraph

	// state ---> actions
	for (unsigned long i = 0; i < sthd->act_num; i++)
	{
	fprintf(output,
			"st%s -> act%sin%s [label=<<font color=\"blue\">%" ACT_FMT "</font>>, color=\"blue\", weight=2.]\n",
			int2String(sthd->st).c_str(), int2String(acts[i]).c_str(), int2String(sthd->st).c_str(), acts[i]);
}

// actions ---> next states
EnvAction_Info *eaif = NULL;
achd = sparser.firstAct();    // restart from beginning
while (achd != NULL)
{
	eaif = sparser.firstEat();
	while (eaif != NULL)
	{
		fprintf(output,
				"act%sin%s -> st%s [label=<<font color=\"red\">%" ACT_FMT " (%" UINT32_FMT ")</font>>, color=\"red\", weight=1.]\n",
				int2String(achd->act).c_str(), int2String(sthd->st).c_str(), int2String(eaif->nst).c_str(),
				eaif->eat, eaif->count);
		// get the payoff of the next state, exclude self
		if (eaif->nst != sthd->st)
		{
			State_Info_Header *nstif = storage->getStateInfo(eaif->nst);
			if (nstif == NULL)    // shouldn't happen
				ERROR("next state: %" ST_FMT " returns NULL!\n", eaif->nst);

			fprintf(output, "st%s [label=\"%" ST_FMT "\\n(%.2f)\"]\n", int2String(eaif->nst).c_str(),
					eaif->nst, nstif->payoff);    // print out next state
			free(nstif);
		}

		eaif = sparser.nextEat();
	}

	achd = sparser.nextAct();
}

free(sthd);
}
else    // state not found
{
fprintf(output, "state %" ST_FMT " not found in memory!\n", st);
}

fprintf(output, "}\n");    // digraph
storage->close();
}

}    // namespace gamcs
