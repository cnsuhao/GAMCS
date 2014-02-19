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


#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>

namespace gimcs
{

// debug on/off
#ifdef _DEBUG_
#define dbgprt(pref, fmt,...)  do{printf("%s ", pref);printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgprt(pref, fmt,...)
#endif

#ifdef _DEBUG_MORE_
#define dbgmoreprt(pref, fmt,...)  do{printf("%s ", pref);printf(fmt,##__VA_ARGS__);}while(0)
#else
#define dbgmoreprt(pref, fmt,...)
#endif

#define ERROR(fmt, ...) do{printf("ERROR:");printf(fmt, ##__VA_ARGS__);exit(-1);}while(0)
#define WARNNING(fmt, ...) do{printf("WARNNING:");printf(fmt, ##__VA_ARGS__);}while(0)

#define UNUSED(expr) do { (void)(expr); } while (0)

void PrintStateInfo(const struct State_Info_Header *);

}
#endif // DEBUG_H
