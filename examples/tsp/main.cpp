/*
 * main.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: andy
 */

#include "saleman.h"
#include "CSAgent.h"

int main(void)
{
    CSAgent ma(1, 0.9, 0.01);
    Saleman saleman("Saleman");
    saleman.ConnectAgent(&ma);
    saleman.Launch();
}



