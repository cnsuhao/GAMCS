/*
 * CSThreadExManager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef CSTHREADMMESSAGER_H_
#define CSTHREADMMESSAGER_H_
#include <pthread.h>
#include <ExManager.h>

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

#endif /* CSTHREADMMESSAGER_H_ */
