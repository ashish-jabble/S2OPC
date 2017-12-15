/*
 *  Copyright (C) 2017 Systerel and others.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
 *
 * \brief Include this file and call SOPC_IEEE_Check to assert IEEE-754 compliance.
 *
 */

#ifndef SOPC_IEEE_CHECK_H_
#define SOPC_IEEE_CHECK_H_

#include <float.h>
#include <stdbool.h>

/**
 * \brief Checks the floating-point compliance to IEEE-754 standard.
 *
 * Most of the verifications are done at compile time with #if statements,
 * but some of them must be done at execution time.
 *
 * \return  true if floating-point arithmetics are compliant to IEEE-754.
 */
bool SOPC_IEEE_Check(void);

#ifndef __clang__ // Clang uses __builtin_flt_rounds() function
#if FLT_ROUNDS != 1
#error "Compiler floating point support is not IEEE-754 compliant"
#endif
#endif

#if FLT_MAX_EXP != 128
#error "Compiler float definition differs from IEEE-754 standard"
#endif

#if FLT_MIN_EXP != (-125)
#error "Compiler float definition differs from IEEE-754 standard"
#endif

#if FLT_MANT_DIG != 24
#error "Compiler float definition differs from IEEE-754 standard"
#endif

#if DBL_MAX_EXP != 1024
#error "Compiler double definition differs from IEEE-754 standard"
#endif

#if DBL_MIN_EXP != (-1021)
#error "Compiler double definition differs from IEEE-754 standard"
#endif

#if DBL_MANT_DIG != 53
#error "Compiler double definition differs from IEEE-754 standard"
#endif

#endif
