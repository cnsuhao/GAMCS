/*

 * main.cpp
 *
 *  Created on: Mar 31, 2014
 *      Author: andy
 */

#include <sys/time.h>
#include <stdio.h>
#include "AnAvatar.h"
#include "gamcs/CSOSAgent.h"

int main(void)
{
    CSOSAgent agent;
    AnAvatar avatar;
    avatar.connectAgent(&agent);

    unsigned long count = 0;
    struct timeval start, end;
    long mtime, seconds, useconds;

    gettimeofday(&start, NULL);
    while (count < 50000)
    {
        avatar.step();

        count++;
    }
    gettimeofday(&end, NULL);

    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
    printf("Elapsed time: %ld milliseconds\n", mtime);

    return 0;
}

