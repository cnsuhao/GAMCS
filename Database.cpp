#include <string.h>
#include "Database.h"

MYSQL *mysql_con;
char SERVER[50];
char USER[50];
char PASSWD[50];
char DATABASE[50] = "MyAgent";
char TABLE[50] = "StateInfo";

int database_connect(char *server, char *user, char *passwd, char *database)
{
    strcpy(SERVER, server);
    strcpy(USER, user);
    strcpy(PASSWD, passwd);
    strcpy(DATABASE, database);

    mysql_con = mysql_init(NULL);

    if (mysql_con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return -1;
    }

    if (mysql_real_connect(mysql_con, SERVER, USER, PASSWD, DATABASE,
                           0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return -1;
    }

    /* create table if not exists */
    char tb_string[256];
    sprintf(tb_string, "CREATE TABLE IF NOT EXISTS %s.%s(State BIGINT PRIMARY KEY, OriPayoff FLOAT, Payoff FLOAT, Count BIGINT, ActInfos BLOB, ExActInfos BLOB, pLinks BLOB) \
            ENGINE INNODB ", DATABASE, TABLE);
    if (mysql_query(mysql_con, tb_string))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return -1;
    }


    return 0;
}

void database_close()
{
    return mysql_close(mysql_con);
}

struct State_Info *db_fetch_state_info(State st)
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", TABLE, st);

    if (mysql_query(mysql_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(mysql_con);

    if (result == NULL)
    {
        printf("no result!\n");
        return NULL;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fields(result);
    if (num_fields != 7)
    {
        printf("Fields don't match!\n");
        return NULL;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        printf("lengths is null\n");
        return NULL;
    }

    int i;
    unsigned long total_len = 0;
    for (i=0; i< num_fields; i++)
    {
        total_len += lengths[i];
    }

    mysql_free_result(result);          // free result

    struct State_Info *stif = (struct State_Info *)malloc(total_len);
    stif->st = atol(row[0]);
    stif->original_payoff = atof(row[1]);
    stif->payoff = atof(row[2]);
    stif->count = atol(row[3]);
    stif->length = total_len;
    unsigned long ai_len = lengths[4];
    stif->act_num = ai_len / sizeof(struct Action_Info);
    unsigned long ea_len = lengths[5];
    stif->eat_num = ea_len / sizeof(struct ExAction_Info);
    unsigned long lk_len = lengths[6];
    stif->lk_num = lk_len / sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    memcpy(p, row[4], ai_len);

    p += ai_len;
    memcpy(p, row[5], ea_len);

    p += ea_len;
    memcpy(p, row[6], lk_len);

    return stif;
}

void db_update_state_info(struct State_Info *stif)
{
    char str[256];
    sprintf(str, "UPDATE %s SET OriPayoff=%.2f, Payoff=%.2f, Count=%ld, ActInfos='%%s', ExActInfos='%%s', pLinks='%%s' WHERE State=%ld",
            TABLE, stif->original_payoff, stif->payoff, stif->count, stif->st);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    p += ea_len;
    struct pLink *lk = (struct pLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(mysql_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(mysql_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(mysql_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

    if (mysql_real_query(mysql_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return;
    }

    return;
}


void db_add_state_info(const struct State_Info *stif)
{
    char str[256];
    sprintf(str, "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, pLinks) VALUES(%ld, %.2f, %.2f, %ld, '%%s', '%%s', '%%s')",
            TABLE, stif->st, stif->original_payoff, stif->payoff, stif->count);
    size_t str_len = strlen(str);

    unsigned long ai_len = stif->act_num * sizeof(struct Action_Info);
    unsigned long ea_len = stif->eat_num * sizeof(struct ExAction_Info);
    unsigned long lk_len = stif->lk_num * sizeof(struct pLink);

    unsigned char *p = (unsigned char *)stif;
    p += sizeof(struct State_Info);
    struct Action_Info *atif = (struct Action_Info *)p;

    p += ai_len;
    struct ExAction_Info *eaif = (struct ExAction_Info *)p;

    p += ea_len;
    struct pLink *lk = (struct pLink *)p;

    char ai_chunk[2*ai_len +1];
    mysql_real_escape_string(mysql_con, ai_chunk, (char *)atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(mysql_con, ea_chunk, (char *)eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(mysql_con, lk_chunk, (char *)lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, str_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

    if (mysql_real_query(mysql_con, query, len))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return;
    }

    return;
}

void db_delete_state_info(State st)
{
    char query_string[256];
    sprintf(query_string, "DELETE  FROM %s WHERE State=%ld", TABLE, st);

    if (mysql_query(mysql_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return;
    }
    return;
}

int db_state_info_exist(State st)
{
    char query_string[256];
    sprintf(query_string, "SELECT * FROM %s WHERE State=%ld", TABLE, st);

    if (mysql_query(mysql_con, query_string))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(mysql_con);

    int re;
    if (result == NULL)
        re = 0;
    else
        re = 1;

    mysql_free_result(result);          // free result
    return re;
}

State db_next_state()
{
    static unsigned long offset = 0;
    char query_str[256];
    sprintf(query_str, "SELECT * FROM %s LIMIT %ld, 1", TABLE, offset);

    if (mysql_query(mysql_con, query_str))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql_con));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(mysql_con);

    if (result == NULL)
    {
        printf("no result!\n");
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        return -1;
    }
    State rs = atol(row[0]);
    offset++;

    mysql_free_result(result);          // free result
    return rs;
}
