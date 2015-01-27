// -----------------------------------------------------------------------------
//
// GAMCS -- Generalized Agent Model and Computer Simulation
//
// Copyright (C) 2013-2014, Andy Huang  <andyspider@126.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// -----------------------------------------------------------------------------
//
// Created on: Apr 3, 2014
//
// -----------------------------------------------------------------------------

#include <cmath>
#include <stdio.h>

#if defined(_WIN32) 
#include <windows.h>    // Sleep
#else
#include <unistd.h>     // usleep
#include <stdlib.h>
#endif

#include "gamcs/platforms.h"

namespace gamcs
{

/**
 * @brief Platform-independent implementation of log2.
 *
 * @param [in] value the value to be calculated
 * @return log2(value)
 */
double pi_log2(double value)
{
#if defined(_WIN32) 
    return log(value) / log(2.0);
#else
    return log2(value);
#endif
}

/**
 * @brief Platform-independent implementation of sleep function.
 *
 * @param [in] ms the millisecond to sleep
 */
void pi_msleep(unsigned long ms)
{
#if defined(_WIN32) 
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

}    // namespace gamcs
