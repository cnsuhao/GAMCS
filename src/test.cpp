#include <stdio.h>
#include <mysql/mysql.h>
#include "Database.h"

MYSQL *db_con;

void DBAddStateInfo(struct State_Info_Header *stif)
{
    char str[256];
    sprintf(str, "INSERT INTO StateInfo (State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, pLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
             stif->st, stif->original_payoff, stif->payoff, stif->count);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info_Header);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    p += ea_len;
    struct pLink *lk = (struct pLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(db_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(db_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(db_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

//    dbgprt("query: %s\n", query);
    if (mysql_real_query(db_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return;
    }

    return;
}

int main(void)
{
    char data[2028];

    int ai_len = 2*sizeof(struct Action_Info);
    int ea_len = 1*sizeof(struct ExAction_Info);
    int lk_len = 3*sizeof(struct pLink);

    struct State_Info_Header stif;
    stif.st = 1;
    stif.original_payoff = 0.1;
    stif.payoff = 0.75;
    stif.count = 198;
    stif.act_num = 2;
    stif.eat_num = 1;
    stif.lk_num = 3;

    memcpy(data, &stif, sizeof(struct State_Info_Header));

    db_con = mysql_init(NULL);
    if (mysql_real_connect(db_con, "localhost", "root", "890127", "test",
                           0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }
    /* create table if not exists */
    char tb_string[256];
    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS test.StateInfo(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, pLinks BLOB) \
            ENGINE  MyISAM");
    if (mysql_query(db_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(db_con));
        return -1;
    }

    for(int i=0; i<1000; i++)
    {
        stif.st = i;
        DBAddStateInfo(&stif);
        printf(".");
        fflush(stdout);
    }

    mysql_close(db_con);
    return 0;
}
