// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Intelligence Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

namespace gamcs
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

void printStateInfo(const struct State_Info_Header *state_information_header);

}
#endif // DEBUG_H
