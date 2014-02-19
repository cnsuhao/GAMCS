/*
 * CSThreadExManager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef CSTHREADEXMANAGER_H
#define CSTHREADEXMANAGER_H
#include <pthread.h>
#include <ExManager.h>

namespace gimcs
{

/**
 *
 */
class CSThreadExManager: public ExManager
{
    public:
        CSThreadExManager();
        CSThreadExManager(int);
        virtual ~CSThreadExManager();

        pthread_t ThreadRun();

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<ExManager *>(args)->Run();
            return NULL;
        }
};

}

#endif /* CSTHREADEXMANAGER_H */
