/*
 * CSThreadExManager.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */
#include <pthread.h>
#include "CSThreadExManager.h"

CSThreadExManager::CSThreadExManager()
{}

CSThreadExManager::CSThreadExManager(int i) : ExManager(i)
{}

CSThreadExManager::~CSThreadExManager()
{}

pthread_t CSThreadExManager::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}
