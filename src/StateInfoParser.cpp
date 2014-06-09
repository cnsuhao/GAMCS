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
// Created on: Feb 28, 2014
//
// -----------------------------------------------------------------------------

#include "gamcs/StateInfoParser.h"

namespace gamcs
{

StateInfoParser::StateInfoParser(const State_Info_Header *sthd) :
		my_sthd(sthd), atp(NULL), act_index(0), eap(NULL), eat_index(0)
{
	atp = (unsigned char *) my_sthd;
	atp += sizeof(struct State_Info_Header);    // point to the first act
	eap = atp + sizeof(EnvAction_Info);    // point to the first eat
}

StateInfoParser::~StateInfoParser()
{
}

/**
 * @brief Get the first action in current state
 * @return address pointer of the action information, or NULL if no any action
 */
Action_Info_Header *StateInfoParser::firstAct()
{
	if (my_sthd->act_num == 0)    // no any act
		return NULL;

	atp = (unsigned char *) my_sthd;    // restart from head
	atp += sizeof(struct State_Info_Header);    // point to the first act
	act_index = 0;    //reindex

	eap = atp + sizeof(Action_Info_Header);    // point to the first eat of act
	eat_index = 0;

	return (Action_Info_Header *) atp;
}

/**
 * @brief Get the next action in current state
 * @return address pointer of the action information, or NULL if no more actions
 */
Action_Info_Header *StateInfoParser::nextAct()
{
	++act_index;
	if (act_index >= my_sthd->act_num)    // no more acts
		return NULL;

	Action_Info_Header *athd = (Action_Info_Header *) atp;    // atp should point to some act header
	atp += sizeof(Action_Info_Header) + athd->eat_num * sizeof(EnvAction_Info);    // point to the next act

	eap = atp + sizeof(Action_Info_Header);    // point to the first eat of act
	eat_index = 0;

	return (Action_Info_Header *) atp;
}

/**
 * @brief Move the pointer to a specified action
 * @param act the requested action
 * @return address pointer of the action information after moving, or NULL if not found
 */
Action_Info_Header *StateInfoParser::move2Act(Agent::Action act)
{
	Action_Info_Header *athd = NULL;

	athd = firstAct();
	while (athd != NULL)
	{
		if (athd->act == act)
			return athd;

		athd = nextAct();
	}

	return NULL;    // not found
}

/**
 * @brief Get the first environment action in current action
 * @return address pointer of the environment action information, or NULL if no any environment action
 */
EnvAction_Info *StateInfoParser::firstEat()
{
	Action_Info_Header *athd = (Action_Info_Header *) atp;    // atp should point to some act header
	if (athd->eat_num == 0)    // no any eat
		return NULL;

	eap = atp + sizeof(Action_Info_Header);    // point to the first eat of act
	eat_index = 0;    // reindex

	return (EnvAction_Info *) eap;
}

/**
 * @brief Get the next environment action in current action
 * @return address pointer of the environment action information, or NULL if no more environment actions
 */
EnvAction_Info *StateInfoParser::nextEat()
{
	Action_Info_Header *athd = (Action_Info_Header *) atp;

	++eat_index;
	if (eat_index >= athd->eat_num)    // no more eats
		return NULL;

	eap += sizeof(EnvAction_Info);    // point to the next eat

	return (EnvAction_Info *) eap;
}

/**
 * @brief Move the pointer to a specified environment action
 * @param eat the requested environment action
 * @return address pointer of the environment action information after moving, or NULL if not found
 */
EnvAction_Info *StateInfoParser::move2Eat(Agent::EnvAction eat)
{
	EnvAction_Info *eaif = NULL;

	eaif = firstEat();
	while (eaif != NULL)
	{
		if (eaif->eat == eat)
			return eaif;

		eaif = nextEat();
	}

	return NULL;    // not found
}

} /* namespace gamcs */
