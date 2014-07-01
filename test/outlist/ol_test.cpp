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

    printf("origin Space Outputs: ");
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
    printf("the first one is %" OUT_FMT "\n", ol.first());
    printf("the last one is %" OUT_FMT "\n", ol.last());

    printf("=============================================\n");

    OSpace ol1(ol);
    printf("copied Space Outputs: ");
    out = ol1.first();
    while (out != GIOM::INVALID_OUTPUT)
    {
        printf("%" OUT_FMT ", ", out);
        out = ol1.next();
    }
    printf("\n");

    printf("size: %" GAMCS_UINT_FMT "\n", ol1.size());
    i = 7;
    printf("index %d is %" OUT_FMT "\n", i, ol1[i]);
    printf("capacity is %" GAMCS_UINT_FMT "\n", ol1.capacity());
    printf("the first one is %" OUT_FMT "\n", ol1.first());
    printf("the last one is %" OUT_FMT "\n", ol1.last());
}
