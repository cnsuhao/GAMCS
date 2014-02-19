// -----------------------------------------------------------------------------
//
// GIMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// -----------------------------------------------------------------------------
//
// Created on: Feb 17, 2014
//
// -----------------------------------------------------------------------------


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
