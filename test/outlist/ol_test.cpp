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
    OutList ol;
    ol.add(1);
    ol.add(2);
    ol.add(3);
    ol.add(10, 14, 2);
    ol.add(20, 100, 1);

//    printf("Outputs: ");
//    for (OutList::iterator out = ol.begin(); out!= ol.end(); ++out)
//    {
//        printf("%ld, ", out);
//    }
//    printf("\n");

    printf("size: %ld\n", ol.size());
    int i = 3;
    printf("index %d is %ld\n", i, ol[i]);
}
