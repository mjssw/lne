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

#ifndef LNE_TIMEVALUE_H
#define LNE_TIMEVALUE_H

#include "config.h"

LNE_NAMESPACE_BEGIN

class LNE_Export TimeValue
{
	friend LNE_Export bool operator < (const TimeValue &tv1, const TimeValue &tv2);
	friend LNE_Export bool operator > (const TimeValue &tv1, const TimeValue &tv2);
	friend LNE_Export bool operator <= (const TimeValue &tv1, const TimeValue &tv2);
	friend LNE_Export bool operator >= (const TimeValue &tv1, const TimeValue &tv2);
	friend LNE_Export bool operator == (const TimeValue &tv1, const TimeValue &tv2);
	friend LNE_Export bool operator != (const TimeValue &tv1, const TimeValue &tv2);

public:
	explicit TimeValue();
	explicit TimeValue(const TimeValue &tv);
	explicit TimeValue(time_t sec, suseconds_t usec = 0);
	explicit TimeValue(const struct timeval &tv);
	explicit TimeValue(const struct timespec &ts);
	void Now();
	void Set(time_t sec, suseconds_t usec = 0);
	void Set(const struct timeval &tv);
	void Set(const struct timespec &ts);

	operator timeval*();
	operator timeval&();
	TimeValue &operator = (const TimeValue &tv);

	LNE_UINT64 ToMillisecond(void) const;
	LNE_UINT64 ToMicroseconds(void) const;
	time_t get_sec(void) const;
	suseconds_t get_usec(void) const;
	operator const timeval*() const ;
	operator const timeval&() const ;
	TimeValue &operator += (const TimeValue &tv);
	TimeValue &operator -= (const TimeValue &tv);

private:
	void Normalize();

	struct timeval tv_;
};

#include "TimeValue.inl"

LNE_NAMESPACE_END

#endif

