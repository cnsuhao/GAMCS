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

#ifndef PLATFORMS_H_
#define PLATFORMS_H_

namespace gamcs
{

/**
 * Platform-independent format strings to print different integer types
 */
#if defined( _WIN32)
#define UINT8_FMT   "u"
#define INT8_FMT    "d"
#define UINT16_FMT  "hu"
#define INT16_FMT   "hd"
#define UINT32_FMT  "I32u"
#define INT32_FMT   "I32d"
#define UINT64_FMT  "I64u"
#define INT64_FMT   "I64d"

#else
#define UINT8_FMT   PRIu8
#define INT8_FMT    PRId8
#define UINT16_FMT  PRIu16
#define INT16_FMT   PRId16
#define UINT32_FMT  PRIu32
#define INT32_FMT   PRId32
#define UINT64_FMT  PRIu64
#define INT64_FMT   PRId64
#endif

double pi_log2(double value);
void pi_msleep(unsigned long ms);

}    // namespace gamcs

#endif /* PLATFORMS_H_ */
