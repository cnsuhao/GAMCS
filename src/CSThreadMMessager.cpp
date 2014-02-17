/*
 * CSThreadMMessager.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSThreadMMessager.h"

CSThreadMMessager::CSThreadMMessager()
{}

CSThreadMMessager::CSThreadMMessager(int i) : MMessager(i)
{}

CSThreadMMessager::~CSThreadMMessager()
{}

pthread_t CSThreadMMessager::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}
