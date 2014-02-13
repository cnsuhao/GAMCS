/*
 * test.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */
#include <assert.h>
#include "Saleman.h"
#include "GIOM.h"

void print_path(int path[CITY_NUM])
{
    for (int i = 0; i < CITY_NUM; i++)
    {
        printf("%d, ", path[i]);
    }
    printf("\n");
}

void decode_state(unsigned long st, int decode_path[CITY_NUM])
{
    for (int i = CITY_NUM - 1; i >= 0; i--)
    {
        unsigned long quotient = st / (CITY_NUM);
        int remainder = st % (CITY_NUM);

        // the ist city
        decode_path[i] = remainder;

        st = quotient;
    }
}

int main(void)
{
    int path[CITY_NUM];
    decode_state(16426592, path);
    print_path(path);

    return 0;
}

