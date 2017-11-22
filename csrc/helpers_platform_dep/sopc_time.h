/**
 *  \file sopc_time.h
 *
 *  \brief Tools for time management
 */
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

#ifndef SOPC_TIME_H_
#define SOPC_TIME_H_

#include "sopc_builtintypes.h"

/**
 *  \brief Suspend current thread execution for (at least) a microsecond interval
 *
 *  \param microsecs  The microsecond interval value for which execution must be suspended
 */
void SOPC_Sleep(unsigned int microsecs);

/**
 * \brief return the current time in DateTime format which is nanoseconds from 1601/01/01 00:00:00 UTC
 *
 * Note: since the clock is not monotonic, it should not be used to measure elapsed time
 *
 * \return the current time in DateTime format
 *
 */
SOPC_DateTime SOPC_Time_GetCurrentTimeUTC(void);

#endif /* SOPC_TIME_H_ */
