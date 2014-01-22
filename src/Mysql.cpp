/*
 * Mysql.cpp
 *
 *  Created on: Jan 21, 2014
 *      Author: andy
 */
#include <mysql/mysql.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Mysql.h"
#include "Debug.h"

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
            "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, BackLinks BLOB) \
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
    mysql_close(db_con);
    return mysql_library_end();
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

/**
 * \brief Get state value from database by specified index.
 * \param index index
 * \return state value of that index, INVALID_STATE for error or not found
 */
Agent::State Mysql::StateByIndex(unsigned long index) const
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
        dbgmoreprt("DBStateByIndex()", "result is  NULL!\n");
        return INVALID_STATE;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("DBStateByIndex()", "lengths is null\n");
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
struct State_Info_Header *Mysql::FetchStateInfo(Agent::State st) const
{
    if (st == INVALID_STATE)
    {
        dbgprt("FetchStateInfo()", "invalid state value\n");
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
        dbgmoreprt("FetchStateInfo()", "result is NULL!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        dbgmoreprt("FetchStateInfo()", "Fields don't match!\n");
        return NULL;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        dbgmoreprt("FetchStateInfo()", "lengths is null\n");
        return NULL;
    }

    // size of actions, env actions and links
    unsigned long ai_len = lengths[4];
    unsigned long ea_len = lengths[5];
    unsigned long lk_len = lengths[6];

    // total size of state info
    int stif_size = sizeof(struct State_Info_Header) + ai_len + ea_len + lk_len;
    // allocate memory and build header
    struct State_Info_Header *stif = (struct State_Info_Header *) malloc(
            stif_size);

    stif->st = atol(row[0]);
    stif->original_payoff = atof(row[1]);
    stif->payoff = atof(row[2]);
    stif->count = atol(row[3]);
    stif->act_num = ai_len / sizeof(struct Action_Info);
    stif->eat_num = ea_len / sizeof(struct EnvAction_Info);
    stif->lk_num = lk_len / sizeof(struct BackLink);
    stif->size = stif_size;

    unsigned char *ptr = (unsigned char *) stif;    // use point ptr to travel through each subpart

    // fill action information part
    ptr += sizeof(struct State_Info_Header);
    memcpy(ptr, row[4], ai_len);

    // fill environment action information part
    ptr += ai_len;
    memcpy(ptr, row[5], ea_len);

    // fill backward links part
    ptr += ea_len;
    memcpy(ptr, row[6], lk_len);

    // move to the end
    ptr += lk_len;

    mysql_free_result(result);    // free result

    if ((ptr - (unsigned char *)stif) != stif_size)    // check size
    {
        ERROR("Mysql FetchStateInfo(): state information header size not match!\n");
    }
    return stif;
}

/**
 * \brief Search for specified state in database.
 * \param state value
 * \return 1 if found, 0 if not
 */
int Mysql::SearchState(Agent::State st) const
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld",
            db_t_stateinfo.c_str(), st);

    if (mysql_query(db_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(db_con);

    int re;
    if (result == NULL)
        re = 0;
    else
        re = 1;

    mysql_free_result(result);    // free result
    return re;
}

/**
 * \brief Add state information to database.
 * \param stif header pointed to state information
 */
void Mysql::AddStateInfo(const struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str,
            "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, BackLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
            db_t_stateinfo.c_str(), stif->st, stif->original_payoff,
            stif->payoff, stif->count);    // first stag of building mysql insert query, actlist, eactlist and links are build below
    size_t str_len = strlen(str);

    // get lenght of several subparts
    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct BackLink);

    unsigned char *p = (unsigned char *) stif;    // use p to travel

    // point to action information part
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;

    // point to environment action information part
    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    // point to backward link part
    p += ea_len;
    struct BackLink *lk = (struct BackLink *) p;

    char ai_chunk[2 * ai_len + 1];    // temporary buffer to put action information
    mysql_real_escape_string(db_con, ai_chunk, (char *) atif, ai_len);
    char ea_chunk[2 * ea_len + 1];    // temporary buffer to put envir action info
    mysql_real_escape_string(db_con, ea_chunk, (char *) eaif, ea_len);
    char lk_chunk[2 * lk_len + 1];    // temporary buffer for links
    mysql_real_escape_string(db_con, lk_chunk, (char *) lk, lk_len);

    char query[str_len + 2 * (ai_len + ea_len + lk_len) + 1];
    int len = snprintf(query, str_len + 2 * (ai_len + ea_len + lk_len) + 1, str,
            ai_chunk, ea_chunk, lk_chunk);    // final stag of building insert query

    if (mysql_real_query(db_con, query, len))    // perform the query, and insert st to database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;    // FIXME: indicator of error?
    }

    return;
}

/**
 * \brief Update information of a state already exists in database.
 * \param stif header pointed to the modified state information
 */
void Mysql::UpdateStateInfo(const struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str,
            "UPDATE %s SET OriPayoff=%.2f, Payoff=%.2f, Count=%ld, ActInfos='%%s', ExActInfos='%%s', BackLinks='%%s' WHERE State=%ld",
            db_t_stateinfo.c_str(), stif->original_payoff, stif->payoff,
            stif->count, stif->st);    // first stage of building the update query
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct EnvAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct BackLink);

    unsigned char *p = (unsigned char *) stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *) p;

    p += ai_len;
    struct EnvAction_Info *eaif = (struct EnvAction_Info *) p;

    p += ea_len;
    struct BackLink *lk = (struct BackLink *) p;

    char ai_chunk[2 * ai_len + 1];
    mysql_real_escape_string(db_con, ai_chunk, (char *) atif, ai_len);
    char ea_chunk[2 * ea_len + 1];
    mysql_real_escape_string(db_con, ea_chunk, (char *) eaif, ea_len);
    char lk_chunk[2 * lk_len + 1];
    mysql_real_escape_string(db_con, lk_chunk, (char *) lk, lk_len);

    char query[str_len + 2 * (ai_len + ea_len + lk_len) + 1];
    int len = snprintf(query, str_len + 2 * (ai_len + ea_len + lk_len) + 1, str,
            ai_chunk, ea_chunk, lk_chunk);    // final stage of building query

    if (mysql_real_query(db_con, query, len))    // perform the query, and update database
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

/**
 * \brief Delete a state from database by its value
 * \param st state value to be delete
 */
void Mysql::DeleteState(Agent::State st)
{
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
            "INSERT INTO %s(TimeStamp, DiscountRate, Threshold, NumStates, NumLinks, LastState, LastAction) VALUES(NULL, %.2f, %.2f, %ld, %ld, %ld, %ld)",
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
struct Memory_Info *Mysql::FetchMemoryInfo()
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
    dbgprt("Mysql FetchMemoryInfo()", "%s, Memory TimeStamp: %s\n", db_name.c_str(), row[0]);
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
