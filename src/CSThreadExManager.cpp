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


#include <pthread.h>
#include "CSThreadExManager.h"

namespace gimcs
{

CSThreadExManager::CSThreadExManager()
{
}

CSThreadExManager::CSThreadExManager(int i) :
        ExManager(i)
{
}

CSThreadExManager::~CSThreadExManager()
{
}

pthread_t CSThreadExManager::ThreadRun()
{
    pthread_t tid;
    pthread_create(&tid, NULL, hook, this);

    return tid;
}

}    // namespace gimcs
