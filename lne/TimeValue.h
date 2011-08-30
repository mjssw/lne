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

#include "BaseObject.h"

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

	operator timeval *();
	operator timeval &();
	TimeValue &operator = (const TimeValue &tv);

	LNE_UINT64 ToMillisecond(void) const;
	LNE_UINT64 ToMicroseconds(void) const;
	time_t sec(void) const;
	suseconds_t usec(void) const;
	operator const timeval *() const ;
	operator const timeval &() const ;
	TimeValue &operator += (const TimeValue &tv);
	TimeValue &operator -= (const TimeValue &tv);

private:
	void Normalize();

	struct timeval tv_;
};

LNE_INLINE
TimeValue::TimeValue()
{
	tv_.tv_sec = 0;
	tv_.tv_usec = 0;
}

LNE_INLINE
TimeValue::TimeValue(const TimeValue &tv)
{
	tv_ = tv.tv_;
}

LNE_INLINE
TimeValue::TimeValue(time_t sec, suseconds_t usec)
{
	Set(sec, usec);
}

LNE_INLINE
TimeValue::TimeValue(const struct timeval &tv)
{
	Set(tv);
}

LNE_INLINE
TimeValue::TimeValue(const struct timespec &ts)
{
	Set(ts);
}

LNE_INLINE void
TimeValue::Now()
{
#if defined(LNE_WIN32)
	struct timeb tb;
	ftime(&tb);
	tv_.tv_sec = (long)tb.time;
	tv_.tv_usec = (long)tb.millitm * 1000;
#else
	gettimeofday(&tv_, NULL);
#endif
}

LNE_INLINE void
TimeValue::Set(time_t sec, suseconds_t usec)
{
	tv_.tv_sec = (long)sec;
	tv_.tv_usec = usec;
}

LNE_INLINE void
TimeValue::Set(const struct timeval &tv)
{
	tv_ = tv;
}

LNE_INLINE void
TimeValue::Set(const struct timespec &ts)
{
	Set(ts.tv_sec, ts.tv_nsec / 1000);
}

LNE_INLINE time_t
TimeValue::sec(void) const
{
	return tv_.tv_sec;
}

LNE_INLINE suseconds_t
TimeValue::usec(void) const
{
	return tv_.tv_usec;
}

LNE_INLINE LNE_UINT64
TimeValue::ToMillisecond(void) const
{
	return (LNE_UINT64)tv_.tv_sec * 1000 + tv_.tv_usec / 1000;
}

LNE_INLINE LNE_UINT64
TimeValue::ToMicroseconds(void) const
{
	return (LNE_UINT64)tv_.tv_sec * 1000 * 1000 + tv_.tv_usec;
}

LNE_INLINE
TimeValue::operator const timeval *() const
{
	return &tv_;
}

LNE_INLINE
TimeValue::operator const timeval &() const
{
	return tv_;
}

LNE_INLINE
TimeValue::operator timeval *()
{
	return &tv_;
}

LNE_INLINE
TimeValue::operator timeval &()
{
	return tv_;
}

LNE_INLINE TimeValue &
TimeValue::operator = (const TimeValue &tv)
{
	tv_ = tv.tv_;
	return *this;
}

LNE_INLINE TimeValue &
TimeValue::operator += (const TimeValue &tv)
{
	tv_.tv_sec += tv.tv_.tv_sec;
	tv_.tv_usec += tv.tv_.tv_usec;
	Normalize();
	return *this;
}

LNE_INLINE TimeValue &
TimeValue::operator -= (const TimeValue &tv)
{
	tv_.tv_sec -= tv.tv_.tv_sec;
	tv_.tv_usec -= tv.tv_.tv_usec;
	Normalize();
	return *this;
}

LNE_INLINE bool
operator > (const TimeValue &tv1, const TimeValue &tv2)
{
	if(tv1.sec() > tv2.sec())
		return true;
	else if(tv1.sec() == tv2.sec()
			&& tv1.usec() > tv2.usec())
		return true;
	else
		return false;
}

LNE_INLINE bool
operator >= (const TimeValue &tv1, const TimeValue &tv2)
{
	if(tv1.sec() > tv2.sec())
		return true;
	else if(tv1.sec() == tv2.sec()
			&& tv1.usec() >= tv2.usec())
		return true;
	else
		return false;
}

LNE_INLINE bool
operator < (const TimeValue &tv1, const TimeValue &tv2)
{
	if(tv1.sec() < tv2.sec())
		return true;
	else if(tv1.sec() == tv2.sec()
			&& tv1.usec() < tv2.usec())
		return true;
	else
		return false;
}

LNE_INLINE bool
operator <= (const TimeValue &tv1, const TimeValue &tv2)
{
	if(tv1.sec() < tv2.sec())
		return true;
	else if(tv1.sec() == tv2.sec()
			&& tv1.usec() <= tv2.usec())
		return true;
	else
		return false;
}

LNE_INLINE bool
operator == (const TimeValue &tv1, const TimeValue &tv2)
{
	return tv1.sec() == tv2.sec() && tv1.usec() == tv2.usec();
}

LNE_INLINE bool
operator != (const TimeValue &tv1, const TimeValue &tv2)
{
	return tv1.sec() != tv2.sec() || tv1.usec() != tv2.usec();
}

LNE_NAMESPACE_END

#endif

