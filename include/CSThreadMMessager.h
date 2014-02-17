/*
 * CSThreadMMessager.h
 *
 *  Created on: Feb 17, 2014
 *      Author: andy
 */

#ifndef CSTHREADMMESSAGER_H_
#define CSTHREADMMESSAGER_H_
#include <pthread.h>
#include <MMessager.h>

/**
 *
 */
class CSThreadMMessager: public MMessager
{
    public:
        CSThreadMMessager();
        CSThreadMMessager(int);
        virtual ~CSThreadMMessager();

        pthread_t ThreadRun();

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<MMessager *>(args)->Run();
            return NULL;
        }
};

#endif /* CSTHREADMMESSAGER_H_ */
