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

#ifndef LNE_THREADCONDITION_H
#define LNE_THREADCONDITION_H

#include "TimeValue.h"
#include "BaseObject.h"

LNE_NAMESPACE_BEGIN

class LNE_Export ThreadCondition: public Available
{
public:
	ThreadCondition(void);
	~ThreadCondition(void);
	LNE_UINT Wait(void);
	LNE_UINT Wait(const TimeValue &tv);;
	LNE_UINT Signal(void);

private:
	ThreadCondition(const ThreadCondition &);
	ThreadCondition &operator=(const ThreadCondition &);

#if defined(LNE_WIN32)
	HANDLE event_;
#else
	bool signal_;
	pthread_mutex_t mutex_;
	pthread_cond_t cond_;
#endif
};

LNE_INLINE LNE_UINT
ThreadCondition::Wait(void)
{
	if(!IsAvailable())
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return WaitForSingleObject(event_, INFINITE) == WAIT_OBJECT_0 ? LNERR_OK : LNERR_UNKNOW;
#else
	int ret = LNERR_UNKNOW;
	if(pthread_mutex_lock(&mutex_) == 0) {
		if(signal_ || pthread_cond_wait(&cond_, &mutex_) == 0) {
			signal_ = false;
			ret = LNERR_OK;
		}
		pthread_mutex_unlock(&mutex_);
	}
	return ret;
#endif
}

LNE_INLINE LNE_UINT
ThreadCondition::Signal(void)
{
	if(!IsAvailable())
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return SetEvent(event_) ? LNERR_OK : LNERR_UNKNOW;
#else
	int ret = LNERR_UNKNOW;
	if(pthread_mutex_lock(&mutex_) == 0) {
		if(signal_)
			ret = LNERR_OK;
		else if(pthread_cond_signal(&cond_) == 0) {
			signal_ = true;
			ret = LNERR_OK;
		}
		pthread_mutex_unlock(&mutex_);
	}
	return ret;
#endif
}

LNE_NAMESPACE_END

#endif
