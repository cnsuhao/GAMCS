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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "gamcs/Sqlite.h"

namespace gamcs
{

Sqlite::Sqlite(std::string dbname) :
		db_con(NULL), db_name(dbname), db_t_stateinfo("StateInfo"), db_t_meminfo(
				"MemoryInfo"), current_index(0), o_flag(O_READ)
{
}

Sqlite::~Sqlite()
{
}

/**
 * @brief Set the database arguments.
 * @param db the database name
 */
void Sqlite::setDBArgs(std::string db)
{
	db_name = db;
}

/**
 * @brief Open the storage for read or write
 * @param flag the open flag
 * @return 0 on successfully opened, or -1 if error occurs
 */
int Sqlite::open(Flag flag)
{
	o_flag = flag;

	int ret = 0;
	char *err_msg;
	if (flag == O_READ)		// open for reading
	{
		ret = sqlite3_open_v2(db_name.c_str(), &db_con, SQLITE_OPEN_READONLY,
		NULL);

		if (ret)	// open error
		{
			fprintf(stderr, "Can't open sqlite for reading, %s!\n",
					sqlite3_errmsg(db_con));
			return ret;
		}

		sqlite3_exec(db_con, "PRAGMA synchronous = NORMAL", NULL, NULL,
				&err_msg);
	}
	else if (flag == O_WRITE)
	{
		// connect to database, create if not exists
		ret = sqlite3_open_v2(db_name.c_str(), &db_con,
				SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

		if (ret)	// open error
		{
			fprintf(stderr, "Can't open sqlite for writing, %s!\n",
					sqlite3_errmsg(db_con));
			return ret;
		}

		sqlite3_exec(db_con, "PRAGMA synchronous = NORMAL", NULL, NULL,
				&err_msg);

		// create tables if not exists
		// state info table
		char tb_string[256];
		sprintf(tb_string,
				"CREATE TABLE IF NOT EXISTS %s(State INTEGER PRIMARY KEY, OriPayoff REAL, Payoff REAL, Count INTEGER, ActNum INTEGER, Size INTEGER, ActInfos BLOB)",
				db_t_stateinfo.c_str());

		ret = sqlite3_exec(db_con, tb_string, NULL, 0, &err_msg);
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "Create stateinfo table failed: %s\n", err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db_con);
			return -1;
		}

		// create memory info table
		sprintf(tb_string,
				"CREATE TABLE IF NOT EXISTS %s(Id INTEGER PRIMARY KEY AUTOINCREMENT, TimeStamp DATETIME DEFAULT CURRENT_TIMESTAMP, DiscountRate REAL, Threshold REAL, NumStates INTEGER, NumLinks INTEGER, LastState INTEGER, LastAction INTEGER)",
				db_t_meminfo.c_str());
		ret = sqlite3_exec(db_con, tb_string, NULL, 0, &err_msg);
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "Create meminfo table failed: %s\n", err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db_con);
			return -1;
		}

		// begin a transaction for writing mode
		sqlite3_exec(db_con, "BEGIN TRANSACTION", NULL, NULL, &err_msg);
	}
	else
	{
		WARNNING("Unknown storage open flag: %d!\n", flag);
		return -1;
	}

	return 0;
}

/**
 * @brief Close the storage
 */
void Sqlite::close()
{
	if (db_con == NULL)
		return;
	else
	{
		if (o_flag == O_WRITE)    // end transaction for writing mode
		{
			sqlite3_exec(db_con, "END TRANSACTION", NULL, NULL, NULL);
		}

		sqlite3_close(db_con);
		db_con = NULL;
	}
}

/**
 * @brief Get the first state in storage
 * @return the first state
 */
Agent::State Sqlite::firstState() const
{
	current_index = 0;
	return stateByIndex(current_index);
}

/**
 * @brief Get the next state in storage
 * @return the next state
 */
Agent::State Sqlite::nextState() const
{
	++current_index;
	return stateByIndex(current_index);
}

/**
 * @brief Get a state by its index in storage
 * @param index the index in storage
 * @return the state at that index, or INVALID_STATE if out of boundary
 */
Agent::State Sqlite::stateByIndex(unsigned long index) const
{
	Agent::State st = Agent::INVALID_STATE;

	char query_str[256];
	sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", db_t_stateinfo.c_str(),
			index);

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, query_str, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "stateByIndex - prepare sql error: #%d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		if (sqlite3_step(stmt) == SQLITE_ROW)    // should only exists on row at most
		{
			st = sqlite3_column_int(stmt, 0);
		}
	}

	sqlite3_finalize(stmt);
	return st;
}

/**
 * @brief Get information of a specified state from storage
 * @param st the requested state
 * @return address point of the state information
 */
struct State_Info_Header *Sqlite::getStateInfo(Agent::State st) const
{
	if (st == Agent::INVALID_STATE)
	{
		dbgprt("Sqlite getStateInfo()", "invalid state value\n");
		return NULL;
	}

	struct State_Info_Header *sthd = NULL;

	char query_string[256];
	sqlite3_stmt *stmt;
	sprintf(query_string, "SELECT * FROM %s WHERE State=%" ST_FMT,
			db_t_stateinfo.c_str(), st);

	int ret = sqlite3_prepare_v2(db_con, query_string, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "getStateInfo - prepare sql error: #%d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			unsigned int sthd_size = sqlite3_column_int(stmt, 5);
			sthd = (State_Info_Header *) malloc(sthd_size);
			sthd->st = sqlite3_column_int(stmt, 0);
			sthd->original_payoff = sqlite3_column_double(stmt, 1);
			sthd->payoff = sqlite3_column_double(stmt, 2);
			sthd->count = sqlite3_column_int(stmt, 3);
			sthd->act_num = sqlite3_column_int(stmt, 4);
			sthd->size = sthd_size;

			unsigned char *stp = (unsigned char *) sthd;
			stp += sizeof(struct State_Info_Header);
			int acif_size = sthd_size - sizeof(State_Info_Header);

			assert(acif_size == sqlite3_column_bytes(stmt, 6));
			memcpy(stp, sqlite3_column_blob(stmt, 6), acif_size);
		}
	}

	sqlite3_finalize(stmt);
	return sthd;
}

/**
 * @brief Check if a state exists in storage
 * @param st the request state
 * @return true|false
 */
bool Sqlite::hasState(Agent::State st) const
{
	bool result = false;

	char query_string[256];
	sprintf(query_string, "SELECT * FROM %s WHERE State=%" ST_FMT,
			db_t_stateinfo.c_str(), st);
	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, query_string, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "hasState - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			result = true;
		}
	}

	sqlite3_finalize(stmt);
	return result;
}

/**
 * @brief Add a state to storage from the given information.
 * When you add new states or links to the memory, make sure to update the memory information correspondingly.
 * Or the memory will stay inconsistent, and the loading will fail.
 * @param sthd the state information
 */
void Sqlite::addStateInfo(const struct State_Info_Header *sthd)
{
	unsigned long act_len = sthd->size - sizeof(State_Info_Header);

	char *stmt_buf = (char *) malloc(512 + 2 * act_len + 3);
	sprintf(stmt_buf,
			"INSERT INTO %s(State, OriPayoff, Payoff, Count, ActNum, Size, ActInfos) VALUES(%" ST_FMT ", %f, %f, %" UINT32_FMT ", %" UINT32_FMT ", %" UINT16_FMT ", ?)",
			db_t_stateinfo.c_str(), sthd->st, sthd->original_payoff,
			sthd->payoff, sthd->count, sthd->act_num, sthd->size);

	char *stp = (char *) sthd;
	stp += sizeof(State_Info_Header);    // point to the first action

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, stmt_buf, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "addStateInfo - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		ret = sqlite3_bind_blob(stmt, 1, stp, act_len, SQLITE_STATIC);    // the leftmost argument is 1
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "addStateInfo - bind blob failed: %s\n",
					sqlite3_errmsg(db_con));
		}
		else
		{
			ret = sqlite3_step(stmt);
			if (ret != SQLITE_DONE)
				fprintf(stderr, "addStateInfo - execution insert failed: %s\n",
						sqlite3_errmsg(db_con));
		}
	}

	sqlite3_finalize(stmt);
	free(stmt_buf);
	return;
}

/**
 * @brief Update a state in storage from the given information
 * When you add new states or links to the memory, make sure to update the memory information correspondingly.
 * Or the memory will stay unconsistent, and the loading will fail.
 * @param sthd the state information
 */
void Sqlite::updateStateInfo(const struct State_Info_Header *sthd)
{
	unsigned long act_len = sthd->size - sizeof(State_Info_Header);

	char *stmt_buf = (char *) malloc(512 + 2 * act_len + 3);

	sprintf(stmt_buf,
			"UPDATE %s SET OriPayoff=%f, Payoff=%f, Count=%" UINT32_FMT ", ActNum=%" UINT32_FMT ", Size=%" UINT16_FMT ", ActInfos=? WHERE State=%" ST_FMT,
			db_t_stateinfo.c_str(), sthd->original_payoff, sthd->payoff,
			sthd->count, sthd->act_num, sthd->size, sthd->st);

	char *stp = (char *) sthd;
	stp += sizeof(State_Info_Header);    // point to the first action

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, stmt_buf, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "updateStateInfo - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		ret = sqlite3_bind_blob(stmt, 1, stp, act_len, SQLITE_STATIC);
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "updateStateInfo - bind blob failed: %s\n",
					sqlite3_errmsg(db_con));
		}
		else
		{
			ret = sqlite3_step(stmt);
			if (ret != SQLITE_DONE)
				fprintf(stderr,
						"updateStateInfo - execution insert failed: %s\n",
						sqlite3_errmsg(db_con));
		}
	}

	sqlite3_finalize(stmt);
	free(stmt_buf);
	return;
}

/**
 * @brief Delete a state from storage
 * @param st the state to be deleted
 * FIXME: need to handle the links with other states!
 */
void Sqlite::deleteState(Agent::State st)
{
	char query_string[256];
	sprintf(query_string, "DELETE FROM %s WHERE State=%" ST_FMT,
			db_t_stateinfo.c_str(), st);    // build delete query

	char *err_msg;
	int ret = sqlite3_exec(db_con, query_string, NULL, 0, &err_msg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "delete state failed: %s\n", err_msg);
	}

	sqlite3_free(err_msg);
	return;
}

/**
 * @brief Add the memory information to storage
 * @param memif the memory information
 */
void Sqlite::addMemoryInfo(const struct Memory_Info *memif)
{
	char query_str[1024];

	sprintf(query_str,
			"INSERT INTO %s(TimeStamp, DiscountRate, Threshold, NumStates, NumLinks, LastState, LastAction) VALUES(datetime(\'now\'), %f, %f, %" UINT32_FMT ", %" UINT32_FMT ", %" ST_FMT ", %" ACT_FMT ")",
			db_t_meminfo.c_str(), memif->discount_rate, memif->threshold,
			memif->state_num, memif->lk_num, memif->last_st, memif->last_act);    // build insert query

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, query_str, -1, &stmt, 0);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "addMemoryInfo - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			fprintf(stderr, "addMemoryInfo - execution insert failed: %d: %s\n",
					ret, sqlite3_errmsg(db_con));
	}

	sqlite3_finalize(stmt);
}

/**
 * @brief Update the memory information in storage
 * @param memif the memory information
 */
void Sqlite::updateMemoryInfo(const struct Memory_Info *memif)
{
	char query_str[1024];

	sprintf(query_str,
			"UPDATE %s SET TimeStamp=datetime(\'now\'), DiscountRate=%f, Threshold=%f, NumStates=%" UINT32_FMT ", NumLinks=%" UINT32_FMT ", LastState=%" ST_FMT ", LastAction=%" ACT_FMT " ORDER BY Id DESC LIMIT 1",
			db_t_meminfo.c_str(), memif->discount_rate, memif->threshold,
			memif->state_num, memif->lk_num, memif->last_st, memif->last_act);

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, query_str, -1, &stmt, 0);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "updateMemoryInfo - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
			fprintf(stderr,
					"updateMemoryInfo - execution update failed: %d: %s\n", ret,
					sqlite3_errmsg(db_con));
	}

	sqlite3_finalize(stmt);
}

/**
 * @brief Get the memory information from storage
 * @return address point of the memory information
 */
struct Memory_Info *Sqlite::getMemoryInfo() const
{
	struct Memory_Info *memif = NULL;

	char query_str[256];
	sprintf(query_str, "SELECT * FROM %s ORDER BY Id DESC LIMIT 1",
			db_t_meminfo.c_str());    // select the lastest one

	sqlite3_stmt *stmt;
	int ret = sqlite3_prepare_v2(db_con, query_str, -1, &stmt, 0);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "getMemoryInfo - prepare sql error: %d: %s\n", ret,
				sqlite3_errmsg(db_con));
	}
	else
	{
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			memif = (struct Memory_Info *) malloc(sizeof(struct Memory_Info));
			memif->discount_rate = sqlite3_column_double(stmt, 2);    // row 1 is Id
			memif->threshold = sqlite3_column_double(stmt, 3);
			memif->state_num = sqlite3_column_int(stmt, 4);
			memif->lk_num = sqlite3_column_int(stmt, 5);
			memif->last_st = sqlite3_column_int(stmt, 6);
			memif->last_act = sqlite3_column_int(stmt, 7);
		}
	}

	sqlite3_finalize(stmt);
	return memif;
}

/**
 * @brief Get the memory name
 * @return the memory name
 */
std::string Sqlite::getMemoryName() const
{
	return db_name;
}

} /* namespace gamcs */
