/*
 * Mysql.h
 *
 *  Created on: Jan 21, 2014
 *      Author: andy
 */
#ifndef MYSQL_H_
#define MYSQL_H_
#include <mysql/mysql.h>
#include <string>
#include "Storage.h"

/**
 * Use Mysql databases as storage.
 */
class Mysql: public Storage
{
    public:
        Mysql() : db_con(NULL), db_server(""), db_user(""), db_password(""), db_name(""),
        db_t_stateinfo("StateInfo"), db_t_meminfo("MemoryInfo")
        {
        }

        ~Mysql()
        {
        }

        int Connect();
        void Close();
        void SetDBArgs(std::string, std::string, std::string, std::string);
        Agent::State StateByIndex(unsigned long) const;
        struct State_Info_Header *FetchStateInfo(Agent::State) const;
        int SearchState(Agent::State) const;
        void AddStateInfo(const struct State_Info_Header *);
        void UpdateStateInfo(const struct State_Info_Header *);
        void DeleteState(Agent::State);
        void AddMemoryInfo(const struct Memory_Info *);
        struct Memory_Info *FetchMemoryInfo();
    private:
        MYSQL *db_con;      /**< database connection handler */
        std::string db_server;   /**< database server address */
        std::string db_user;     /**< database username */
        std::string db_password; /**< database password */
        std::string db_name;     /**< database name */
        std::string db_t_stateinfo;  /**< table name for storing state information */
        std::string db_t_meminfo;    /**< table name for storing memory information */
};

#endif /* MYSQL_H_ */
