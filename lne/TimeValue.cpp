/*
 *  Copyright (C) 2011  Vietor Liu <vietor.liu@gmail.com>
 *
 *  This file is part of LNE.
 *  LNE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Lesser Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LNE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with LNE.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TimeValue.h"

LNE_NAMESPACE_USING

const static suseconds_t ONE_SECOND_IN_USECS = 1000000;

void TimeValue::Normalize()
{
	if(tv_.tv_usec >= ONE_SECOND_IN_USECS) {
		do {
			++tv_.tv_sec;
			tv_.tv_usec -= ONE_SECOND_IN_USECS;
		} while(tv_.tv_usec >= ONE_SECOND_IN_USECS);
	} else if(tv_.tv_usec <= -ONE_SECOND_IN_USECS) {
		do {
			--tv_.tv_sec;
			tv_.tv_usec += ONE_SECOND_IN_USECS;
		} while(tv_.tv_usec <= -ONE_SECOND_IN_USECS);
	}
	if(tv_.tv_sec >= 1 && tv_.tv_usec < 0) {
		--tv_.tv_sec;
		tv_.tv_usec += ONE_SECOND_IN_USECS;
	}
}
