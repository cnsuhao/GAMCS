// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------
//
// Created on: Jan 21, 2014
//
// -----------------------------------------------------------------------------


#ifndef MYSQL_H_
#define MYSQL_H_
#include <mysql/mysql.h>
#include <string>
#include "Storage.h"

namespace gimcs
{

/**
 * Use Mysql databases as storage.
 */
class Mysql: public Storage
{
    public:
        Mysql() :
                db_con(NULL), db_server(""), db_user(""), db_password(""), db_name(
                        ""), db_t_stateinfo("StateInfo"), db_t_meminfo(
                        "MemoryInfo"), current_index(0)
        {
        }

        ~Mysql()
        {
        }

        int Connect();
        void Close();
        void SetDBArgs(std::string, std::string, std::string, std::string);

        Agent::State FirstState() const;
        Agent::State NextState() const;
        bool HasState(Agent::State) const;

        struct State_Info_Header *GetStateInfo(Agent::State) const;
        void AddStateInfo(const struct State_Info_Header *);
        void UpdateStateInfo(const struct State_Info_Header *);
        void DeleteState(Agent::State);

        void AddMemoryInfo(const struct Memory_Info *);
        struct Memory_Info *GetMemoryInfo() const;
        std::string GetMemoryName() const;

    private:
        MYSQL *db_con; /**< database connection handler */
        std::string db_server; /**< database server address */
        std::string db_user; /**< database username */
        std::string db_password; /**< database password */
        std::string db_name; /**< database name */
        std::string db_t_stateinfo; /**< table name for storing state information */
        std::string db_t_meminfo; /**< table name for storing memory information */
        mutable unsigned long current_index;

        Agent::State StateByIndex(unsigned long) const;
};

inline std::string Mysql::GetMemoryName() const
{
    return db_name;
}

}    // namespace gimcs
#endif /* MYSQL_H_ */
