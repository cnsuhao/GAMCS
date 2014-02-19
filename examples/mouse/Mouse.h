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


#ifndef MOUSE_H_
#define MOUSE_H_
#include <string>
#include "Avatar.h"

using namespace gimcs;

class Mouse: public Avatar
{
    public:
        /** Default constructor */
        Mouse(std::string);
        /** Default destructor */
        ~Mouse();

    private:
        Agent::State position;
        int count;

        Agent::State GetCurrentState();
        void PerformAction(Agent::Action);
        OSpace ActionCandidates(Agent::State);
        float OriginalPayoff(Agent::State);
};

#endif // MOUSE_H_
