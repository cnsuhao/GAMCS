/*
 * test.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */
#include <assert.h>
#include "saleman.h"

void print_path(int path[CITY_NUM])
{
    for (int i=0; i<CITY_NUM; i++)
    {
        printf("%d, ", path[i]);
    }
    printf("\n");
}

int main(void)
{
    int path[CITY_NUM] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int tmp_path[CITY_NUM];
    int acts[CITY_NUM] = {-1,2, 7, 1, -8, 1, -3, -6, -5, 4};
    int tmp_acts[CITY_NUM];
    unsigned long state1, state2 = 10987654321, state3 = 517349;
    long action1, action2 = -119;

    Saleman saleman("Saleman");
    saleman.EncodeState(path, state1);
    printf("%ld\n", state1);
    saleman.DecodeState(state2, tmp_path);
    print_path(tmp_path);
}



