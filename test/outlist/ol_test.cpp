/*
 * ol_test.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include <stdio.h>
#include "gamcs/GIOM.h"

using namespace gamcs;

int main(void)
{
    OSpace ol;
    ol.add(1);
    ol.add(2);
    ol.add(3);
    ol.add(0, 80, 1);
    ol.add(20, 10, -1);

    printf("Outputs: ");
    GIOM::Output out = ol.first();
    while (out != GIOM::INVALID_OUTPUT)
    {
        printf("%" OUT_FMT ", ", out);
        out = ol.next();
    }
    printf("\n");

    printf("size: %" GAMCS_UINT_FMT "\n", ol.size());
    int i = 7;
    printf("index %d is %" OUT_FMT "\n", i, ol[i]);
    printf("capacity is %" GAMCS_UINT_FMT "\n", ol.capacity());

    gamcs_uint j = 9999;
    printf("gamcs_uint size: %d\n", sizeof(j));
}
