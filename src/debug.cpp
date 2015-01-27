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

#include <string.h>
#include "gamcs/debug.h"
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 * @brief Pretty print state information.
 *
 * @param [in] sthd the specified State information header
 */
void printStateInfo(const struct State_Info_Header *sthd)
{
	if (sthd == NULL)
		return;

	printf("++++++++++++++++++++++++ State: %" ST_FMT " ++++++++++++++++++++++++++\n",
			sthd->st);
	printf("Original payoff: %.2f,\t Payoff: %.2f,\t Count: %" UINT32_FMT ", ActNum: %" UINT32_FMT "\n",
			sthd->original_payoff, sthd->payoff, sthd->count, sthd->act_num);
	printf("------------------------------------------------------------\n");
	unsigned char *stp = (unsigned char *) sthd;    // use point stp to travel through each subpart of state
	unsigned char *atp;
	// environment action information
	stp += sizeof(struct State_Info_Header);    // point to the first act
	unsigned long anum;
	for (anum = 0; anum < sthd->act_num; anum++)
	{
		Action_Info_Header *athd = (Action_Info_Header *) stp;
		atp = stp;
		atp += sizeof(Action_Info_Header);    // point to the first eat of act
		unsigned long i;
		for (i = 0; i < athd->eat_num; i++)    // print every eat of this act
		{
			EnvAction_Info *eaif = (EnvAction_Info *) atp;
			printf(
					"\t  .|+++ %" ACT_FMT " +++ %" ACT_FMT " ++> %" ST_FMT " \t Count: %" UINT32_FMT "\n",
					athd->act, eaif->eat, eaif->nst, eaif->count);

			atp += sizeof(EnvAction_Info);    // point to the next eat
		}
		stp += sizeof(Action_Info_Header)
				+ athd->eat_num * sizeof(EnvAction_Info);    // point to the next act
	}

	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

	return;
}

/**
 * @brief Show a progress bar in console.
 *
 * @param [in] index current index
 * @param [in] total the total amount
 * @param [in] label the label used to show in the front of progress bar
 */
void consoleProgressBar(unsigned long index, unsigned long total, char *label)
{
#if defined(_WIN32)
	printf("%3d %%\n", (int) (1.0 * index / total));
	return;
#else
	double prcnt;
	int num_of_dots;
	char buffer[80] =
	{ 0 };
	int width;
	/* get term width */
	FILE *fp;
	prcnt = 1.0 * index / total;
	fp = popen("stty size | cut -d\" \" -f2", "r");
	if (fp == NULL)
		return;
	if (fgets(buffer, sizeof(buffer), fp) == NULL)
		return;
	pclose(fp);
	width = atoi(buffer);

	if (width < 32)
	{
		printf("\e[1A%3d%% completed.\n", (int) (prcnt * 100));
	}
	else
	{
		num_of_dots = width - 20;

		char *pline_to_print = (char *) malloc(sizeof(char) * width);
		int dots = (int) (num_of_dots * prcnt);

		memset(pline_to_print, 0, width);
		memset(pline_to_print, '>', dots);
		memset(pline_to_print + dots, ' ', num_of_dots - dots);
		printf("\e[1A%s[%s] %3d%% \n", label, pline_to_print,
				(int) (prcnt * 100));
		free(pline_to_print);
	}
	return;
#endif
}

}    // namespace gamcs

