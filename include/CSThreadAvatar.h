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
// Created on: Jan 20, 2014
//
// -----------------------------------------------------------------------------


#ifndef CSTHREADAVATAR_H_
#define CSTHREADAVATAR_H_
#include <pthread.h>
#include <string>
#include "Avatar.h"

namespace gimcs
{

/**
 * Computer Simulation Avatar of Agent.
 */
class CSThreadAvatar: public Avatar
{
    public:
        CSThreadAvatar();
        CSThreadAvatar(std::string);
        virtual ~CSThreadAvatar();

        pthread_t ThreadLaunch(); /**< if it's a group, launch each of its  members in a thread */

    private:
        static void* hook(void* args)
        { /**< hook to run a class function(Launch() here) in a thread */
            reinterpret_cast<Avatar *>(args)->Launch();
            return NULL;
        }
};

}    // namespace gimcs
#endif /* CSTHREADAVATAR_H_ */
