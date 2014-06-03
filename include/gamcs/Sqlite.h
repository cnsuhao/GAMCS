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
// Created on: May 16, 2014
//
// -----------------------------------------------------------------------------

#ifndef SQLITE_H_
#define SQLITE_H_
#include <sqlite3.h>
#include <string>
#include "gamcs/Storage.h"

namespace gamcs
{

/**
 * Sqlite database as storage
 */
class Sqlite: public Storage
{
public:
	Sqlite(std::string database = "");
	~Sqlite();

	void setDBArgs(std::string database);

	int open(Flag flag);
	void close();

	Agent::State firstState() const;
	Agent::State nextState() const;
	bool hasState(Agent::State state) const;

	struct State_Info_Header *getStateInfo(Agent::State state) const;
	void addStateInfo(const struct State_Info_Header *state_information_header);
	void updateStateInfo(
			const struct State_Info_Header *state_information_header);
	DEPRECATED("This function is not completely supported yet and will easily lead to storage inconsistent!\n")
	void deleteState(Agent::State state);

	struct Memory_Info *getMemoryInfo() const;
	void addMemoryInfo(const struct Memory_Info *memory_information_header);
	void updateMemoryInfo(const struct Memory_Info *memory_information_header);
	std::string getMemoryName() const;

private:
	sqlite3 *db_con;
	std::string db_name;
	std::string db_t_stateinfo;
	std::string db_t_meminfo;
	mutable gamcs_uint current_index;
	Flag o_flag;

	Agent::State stateByIndex(unsigned long index) const;
};

} /* namespace gamcs */

#endif /* SQLITE_H_ */
