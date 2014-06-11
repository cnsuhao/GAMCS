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
// Created on: Jan 21, 2014
//
// -----------------------------------------------------------------------------

#ifndef MYSQL_H_
#define MYSQL_H_
#if defined( _WIN32)
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif
#include <string>
#include "gamcs/Storage.h"

namespace gamcs
{

/**
 * @brief Mysql storage.
 *
 * Use a mysql database as storage to dump/load agent memory.
 */
class Mysql: public Storage
{
	public:
		Mysql(std::string server = "", std::string user = "",
				std::string password = "", std::string database = "");
		~Mysql();

		void setDBArgs(std::string server, std::string user,
				std::string password, std::string database);

		int open(Flag flag);
		void close();

		Agent::State firstState() const;
		Agent::State nextState() const;
		bool hasState(Agent::State state) const;

		struct State_Info_Header *getStateInfo(Agent::State state) const;
		void addStateInfo(
				const struct State_Info_Header *state_information_header);
		void updateStateInfo(
				const struct State_Info_Header *state_information_header);
		DEPRECATED("This function is not completely supported yet and will easily lead to storage inconsistent!\n")
		void deleteState(Agent::State state);

		struct Memory_Info *getMemoryInfo() const;
		void addMemoryInfo(const struct Memory_Info *memory_information_header);
		void updateMemoryInfo(
				const struct Memory_Info *memory_information_header);
		std::string getMemoryName() const;

	private:
		MYSQL *db_con; /**< the mysql connection handler */
		std::string db_server; /**< mysql server address */
		std::string db_user; /**< mysql username */
		std::string db_password; /**< mysql password */
		std::string db_name; /**< database name */
		std::string db_t_stateinfo; /**< table name for storing state information */
		std::string db_t_meminfo; /**< table name for storing memory information */
		mutable gamcs_uint current_index; /**< index used by iterator */

		Agent::State stateByIndex(unsigned long index) const;
};

}    // namespace gamcs
#endif /* MYSQL_H_ */
