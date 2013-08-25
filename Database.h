#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Agent.h"

int database_connect(char *, char *, char *, char *);
void database_close();

State db_next_state();
int db_search_state_info(State);
struct State_Info *db_fetch_state_info(State);
void db_add_state_info(const struct State_Info *);
void db_update_state_info(struct State_Info *);
void db_delete_state_info(State);

#endif // DATABASE_H
