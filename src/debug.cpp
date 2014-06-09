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

#include "gamcs/debug.h"
#include "gamcs/Agent.h"

namespace gamcs
{

/**
 * @brief Pretty print state information
 * @param sthd the specified State information header
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

}    // namespace gamcs

