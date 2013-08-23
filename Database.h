#ifndef DATABASE_H
#define DATABASE_H
#include <mysql.h>
#include "Agent.h"

int database_connect();
void database_close();

struct State_Info *fetch_state_info(State);
void add_state_info(struct State_Info *);
void update_state_info(struct State_Info *);
void delete_state_info(State);

#endif // DATABASE_H
