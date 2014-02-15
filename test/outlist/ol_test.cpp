/*
 * ol_test.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include "stdio.h"
#include "GIOM.h"

int main(void)
{
    OSpace ol;
    ol.add(1);
    ol.add(2);
    ol.add(3);
    ol.add(10, 14, 2);
    ol.add(20, 10, 1);

    printf("Outputs: ");
    GIOM::Output out = ol.first();
    while (out != INVALID_OUTPUT)
    {
        printf("%ld, ", out);
        out = ol.next();
    }
    printf("\n");

    printf("size: %ld\n", ol.size());
    int i = 7;
    printf("index %d is %ld\n", i, ol[i]);
    printf("capacity is %ld\n", ol.capacity());
}
