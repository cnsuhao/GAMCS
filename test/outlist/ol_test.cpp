/*
 * ol_test.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include <stdio.h>
#include "gimcs/GIOM.h"

using namespace gimcs;

int main(void)
{
    OSpace ol;
    ol.Add(1);
    ol.Add(2);
    ol.Add(3);
    ol.Add(10, 14, 2);
    ol.Add(20, 10, -1);

    printf("Outputs: ");
    GIOM::Output out = ol.First();
    while (out != GIOM::INVALID_OUTPUT)
    {
        printf("%" OUT_FMT ", ", out);
        out = ol.Next();
    }
    printf("\n");

    printf("size: %" GIMCS_UINT_FMT "\n", ol.Size());
    int i = 7;
    printf("index %d is %" OUT_FMT "\n", i, ol[i]);
    printf("capacity is %" GIMCS_UINT_FMT "\n", ol.Capacity());

    gimcs_uint j = 9999;
    printf("gimcs_uint size: %d\n", sizeof(j));
}
