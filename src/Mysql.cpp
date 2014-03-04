// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gimcs/Mysql.h"
#include "gimcs/Debug.h"

namespace gimcs
{

/**
 * \brief Connect to database.
 * \return -1 for error, 0 for success
 */
int Mysql::Connect()
{
    if (mysql_library_init(0, NULL, NULL))
    {
        fprintf(stderr, "could not initialize MySQL library!\n");
        return -1;
    }

    db_con = mysql_init(NULL);

    if (db_con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    if (mysql_real_connect(db_con, db_server.c_str(), db_user.c_str(),
            db_password.c_str(), NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    /* create database if not exists */
    char db_string[128];
    sprintf(db_string, "CREATE DATABASE IF NOT EXISTS %s", db_name.c_str());
    if (mysql_query(db_con, db_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    mysql_select_db(db_con, db_name.c_str());    // use database
    /* create table if not exists */
    char tb_string[256];
    sprintf(tb_string,
            "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActNum BIGINT, Size INT, ActInfos BLOB) \
            ENGINE MyISAM ",
            db_name.c_str(), db_t_stateinfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    sprintf(tb_string,
            "CREATE TABLE IF NOT EXISTS %s.%s(TimeStamp TIMESTAMP PRIMARY KEY, DiscountRate FLOAT, Threshold FLOAT, NumStates BIGINT, NumLinks BIGINT, LastState BIGINT, LastAction BIGINT) \
            ENGINE MyISAM ",
            db_name.c_str(), db_t_meminfo.c_str());
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    return 0;
}

/**
 * \brief Close database.
 */
void Mysql::Close()
{
    if (db_con == NULL)
        return;
    else
    {
        mysql_close(db_con);
        mysql_library_end();
        db_con = NULL;
    }
}

/**
 * \brief Set arguments for connecting database.
 * \param srv database server location
 * \param usr username of database
 * \param passwd password of username
 * \param name of the database
 */
void Mysql::SetDBArgs(std::string srv, std::string usr, std::string passwd,
        std::string db)
{
    db_server = srv;
    db_user = usr;
    db_password = passwd;
    db_name = db;
    return;
}

Agent::State Mysql::FirstState() const
{
    current_index = 0;
    return StateByIndex(current_index);
}

Agent::State Mysql::NextState() const
{
    current_index++;
    return StateByIndex(current_index);
}

/**
 * \brief Get state value from database by specified index.
 * \param index index
 * \return state value of that index, INVALID_STATE for error or not found
 */
Agent::State Mysql::StateByIndex(uint64_t index) const
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", db_t_stateinfo.c_str(),
            index);

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return INVALID_STATE;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("StateByIndex()", "result is  NULL!\n");
        return INVALID_STATE;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("StateByIndex()", "lengths is null\n");
        mysql_free_result(result);
        return INVALID_STATE;
    }
    Agent::State rs = atol(row[0]);

    mysql_free_result(result);    // free result
    return rs;
}

/**
 * \brief Fetch state information from database.
 * \param st state value
 * \return fetched state information, NULL if error
 */
struct State_Info_Header *Mysql::GetStateInfo(Agent::State st) const
{
    if (st == INVALID_STATE)
    {
        dbgprt("Mysql GetStateInfo()", "invalid state value\n");
        return NULL;
    }

    // do mysql query
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld",
            db_t_stateinfo.c_str(), st);    // build mysql query

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("Mysql GetStateInfo()", "result is NULL!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        dbgmoreprt("Mysql GetStateInfo()", "Field number doesn't match!\n");
        return NULL;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("Mysql GetStateInfo()", "lengths is null\n");
        return NULL;
    }

    unsigned int sthd_size = atoi(row[5]);
    State_Info_Header *sthd = (State_Info_Header *) malloc(sthd_size);
    sthd->st = atol(row[0]);
    sthd->original_payoff = atof(row[1]);
    sthd->payoff = atof(row[2]);
    sthd->count = atol(row[3]);
    sthd->act_num = atoi(row[4]);
    sthd->size = sthd_size;

    unsigned char *stp = (unsigned char *) sthd;
    stp += sizeof(struct State_Info_Header);    // point to the first act
    unsigned int acif_size = sthd_size - sizeof(State_Info_Header);

    assert(acif_size == lengths[6]);    // check
    memcpy(stp, row[6], acif_size);    // copy action infos

    mysql_free_result(result);
    return sthd;
}

/**
 * \brief Search for specified state in database.
 * \param state value
 * \return 1 if found, 0 if not
 */
bool Mysql::HasState(Agent::State st) const
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld",
            db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return false;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    int re;
    if (result->row_count == 0)    // 0 row found
        re = false;
    else
        re = true;

    mysql_free_result(result);    // free result
    return re;
}

/**
 * \brief Add state information to database.
 * \param stif header pointed to state information
 */
void Mysql::AddStateInfo(const struct State_Info_Header *sthd)
{
    char str[256];
    sprintf(str,
            "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActNum, Size, ActInfos) VALUES(%ld, %f, %f, %ld, %ld, %d,'%%s')",
            db_t_stateinfo.c_str(), sthd->st, sthd->original_payoff,
            sthd->payoff, sthd->count, sthd->act_num, sthd->size);    // first stag of building mysql insert query, actlist, eactlist and links are build below
    size_t str_len = strlen(str);

    // get length of actinfos
    unsigned long act_len = sthd->size - sizeof(State_Info_Header);

    unsigned char *stp = (unsigned char *) sthd;
    stp += sizeof(struct State_Info_Header);    // point to the first act
    char acif_chunk[2 * act_len + 1];    // temporary buffer to put envir action info
    mysql_real_escape_string(db_con, acif_chunk, (char *) stp, act_len);

    char query[str_len + 2 * act_len + 1];
    int len = snprintf(query, str_len + 2 * act_len + 1, str, acif_chunk);    // final stage of building insert query
    if (mysql_real_query(db_con, query, len))    // perform the query, and insert st to database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
    }

    return;
}

/**
 * \brief Update information of a state already exists in database.
 * \param stif header pointed to the modified state information
 */
void Mysql::UpdateStateInfo(const struct State_Info_Header *sthd)
{
    char str[256];
    sprintf(str,
            "UPDATE %s SET OriPayoff=%f, Payoff=%f, Count=%ld, ActNum=%ld, Size=%d, ActInfos='%%s' WHERE State=%ld",
            db_t_stateinfo.c_str(), sthd->original_payoff, sthd->payoff,
            sthd->count, sthd->act_num, sthd->size, sthd->st);    // first stage of building the update query
    size_t str_len = strlen(str);

    // get length of actinfos
    unsigned long act_len = sthd->size - sizeof(State_Info_Header);

    unsigned char *stp = (unsigned char *) sthd;
    stp += sizeof(struct State_Info_Header);    // point to the first act
    char acif_chunk[2 * act_len + 1];    // temporary buffer to put envir action info
    mysql_real_escape_string(db_con, acif_chunk, (char *) stp, act_len);

    char query[str_len + 2 * act_len + 1];
    int len = snprintf(query, str_len + 2 * act_len + 1, str, acif_chunk);    // final stage of building insert query
    if (mysql_real_query(db_con, query, len))    // perform the query, and insert st to database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
    }

    return;
}

/**
 * \brief Delete a state from database by its value
 * \param st state value to be delete
 * FIXME: need to handle the links with other states!
 */
void Mysql::DeleteState(Agent::State st)
{
    WARNNING(
            "DeleteState() is not completely implemented yet, it's buggy and will not work as expected, DON'T use it!\n");

    char query_string[256];
    sprintf(query_string, "DELETE  FROM %s WHERE State=%ld",
            db_t_stateinfo.c_str(), st);    // build delete query

    if (mysql_query(db_con, query_string))    // perform query
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

/**
 * \brief Add memory statistics to database.
 */
void Mysql::AddMemoryInfo(const struct Memory_Info *memif)
{
    char query_str[256];

    sprintf(query_str,
            "INSERT INTO %s(TimeStamp, DiscountRate, Threshold, NumStates, NumLinks, LastState, LastAction) VALUES(NULL, %f, %f, %ld, %ld, %ld, %ld)",
            db_t_meminfo.c_str(), memif->discount_rate, memif->threshold,
            memif->state_num, memif->lk_num, memif->last_st, memif->last_act);    // build insert query

    int len = strlen(query_str);
    if (mysql_real_query(db_con, query_str, len))    // perform query
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }
    return;
}

/**
 * \brief Fetch memory statistics from database.
 * \return memory info struct, NULL if error
 */
struct Memory_Info *Mysql::GetMemoryInfo() const
{
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s ORDER BY TimeStamp DESC LIMIT 1",
            db_t_meminfo.c_str());    // select the lastest one

    if (mysql_query(db_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    if (result == NULL)
    {
        dbgmoreprt("Mysql FetchMemoryInfo()", "result is NULL!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("Mysql FetchMemoryInfo()", "lengths is null\n");
        mysql_free_result(result);
        return NULL;
    }

    struct Memory_Info *memif = (struct Memory_Info *) malloc(
            sizeof(struct Memory_Info));
    dbgmoreprt("Mysql FetchMemoryInfo()", "%s, Memory TimeStamp: %s\n",
            db_name.c_str(), row[0]);
    // fill in the memory struct
    memif->discount_rate = atof(row[1]);
    memif->threshold = atof(row[2]);
    memif->state_num = atol(row[3]);
    memif->lk_num = atol(row[4]);
    memif->last_st = atol(row[5]);
    memif->last_act = atol(row[6]);

    mysql_free_result(result);    // free result

    return memif;
}

}    // namespace gimcs
