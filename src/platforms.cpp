// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// Created on: Apr 3, 2014
//
// -----------------------------------------------------------------------------

#include <cmath>
#include <stdio.h>

#if defined(_WIN32) 
#include <windows.h>    // Sleep
#else
#include <unistd.h>     // usleep
#include <string.h>
#include <stdlib.h>
#endif

#include "gamcs/platforms.h"

namespace gamcs
{

double pi_log2(double value)
{
#if defined(_WIN32) 
    return log(value) / log(2.0);
#else
    return log2(value);
#endif
}

void pi_msleep(unsigned long ms)
{
#if defined(_WIN32) 
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void pi_progressBar(unsigned long index, unsigned long total, char *label)
{
#if defined(_WIN32) 
    printf("%3d %%\n", (int) (1.0 * index / total));
    return;
#else
    double prcnt;
    int num_of_dots;
    char buffer[80] = { 0 };
    int width;
    /* get term width */
    FILE *fp;
    prcnt = 1.0 * index / total;
    fp = popen("stty size | cut -d\" \" -f2", "r");
    if (fgets(buffer, sizeof(buffer), fp) == NULL) return;
    pclose(fp);
    width = atoi(buffer);

    if (width < 32)
    {
        printf("\e[1A%3d%% completed.\n", (int) (prcnt * 100));
    }
    else
    {
        num_of_dots = width - 20;

        char *pline_to_print = (char *) malloc(sizeof(char) * width);
        int dots = (int) (num_of_dots * prcnt);

        memset(pline_to_print, 0, width);
        memset(pline_to_print, '>', dots);
        memset(pline_to_print + dots, ' ', num_of_dots - dots);
        printf("\e[1A%s[%s] %3d%% \n", label, pline_to_print,
                (int) (prcnt * 100));
        free(pline_to_print);
    }
    return;
#endif
}

}    // namespace gamcs
