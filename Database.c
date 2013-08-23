#include "Database.h"

MYSQL *mysql_con;
char SERVER[50];
char USER[50];
char PASSWD[50];
char DATABASE[50] = "MyAgent";
char TABLE[50] = "StateInfo";

int database_connect(char *server, char *user, char *passwd)
{
    strcpy(SERVER, server);
    strcpy(USER, user);
    strcpy(PASSWD, passwd);

    MYSQL *mysql_con = mysql_init(NULL);

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

    return 0;
}

void database_close()
{
    return mysql_close(mysql_con);
}

struct State_Info *fetch_state_info(State st)
{
    char query_string[128];
    sprintf(query_string, "SELECT FROM %s WHERE State=%d", TABLE, st);

    if (mysql_query(mysql_con, query_string))
    {
        finish_with_error(mysql_con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        finish_with_error(mysql_con);
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int num_fields = mysql_num_fileds(result);
    if (num_fields != 7)
    {
        printf("Fields don't match!\n");
        return NULL;
    }
    unsigned long *lengths = mysql_fetch_lengths(result);

    if (lengths == NULL)
    {
        finish_with_error(mysql_con);
    }

    int i;
    unsigned long total_len = 0;
    for (i=0; i< num_fields; i++)
    {
        total_len += lengths[i];
    }

    struct State_Info *stif = (struct State_Info *)malloc(total_len);
    stif->st = row[0];
    stif->original_payoff = row[1];
    stif->payoff = row[2];
    stif->count = row[3];
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

    mysql_free_result(result);
    return stif;
}

void add_state_info(struct State_Info *stif)
{
    char str[128];
    sprintf(str, "INSERT INTO %s(State, OriPayoff, Payoff, Count, ActInfos, ExActInfos, pLinks) VALUES(%ld, %.2f, %.2f, %ld, '%s, %s, %s')",
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
    mysql_real_escape_string(mysql_con, ai_chunk, atif, ai_len);
    char ea_chunk[2*ea_len +1];
    mysql_real_escape_string(mysql_con, ea_chunk, eaif, ea_len);
    char lk_chunk[2*lk_len +1];
    mysql_real_escape_string(mysql_con, lk_chunk, lk, lk_len);

    char query[str_len + 2*(ai_len+ea_len+lk_len)+1];
    int len = snprintf(query, st_len + 2*(ai_len+ea_len+lk_len)+1, str, ai_chunk, ea_chunk, lk_chunk);

    if (mysql_real_query(mysql_con, query, len))
    {
        finish_with_error(mysql_con);
    }

    return;
}
