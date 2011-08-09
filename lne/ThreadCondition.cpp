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

#include "ThreadCondition.h"

LNE_NAMESPACE_USING

ThreadCondition::ThreadCondition(void)
{
#if defined(LNE_WIN32)
	event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	initialized_ = event_ != NULL;
#else
	signal_ = false;
	pthread_mutex_init(&mutex_, NULL);
	pthread_cond_init(&cond_, NULL);
	initialized_ = true;
#endif
}

ThreadCondition::~ThreadCondition(void)
{
	if(initialized_) {
#if defined(LNE_WIN32)
		CloseHandle(event_);
#else
		pthread_cond_destroy(&cond_);
		pthread_mutex_destroy(&mutex_);
#endif
	}
}

LNE_UINT ThreadCondition::Wait(const TimeValue &tv)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	DWORD ret = WaitForSingleObject(event_, (DWORD)tv.ToMillisecond());
	if(ret == WAIT_OBJECT_0)
		return LNERR_OK;
	if(ret == WAIT_TIMEOUT)
		return LNERR_TIMEOUT;
	return LNERR_UNKNOW;
#else
	struct timespec ts;
	TimeValue dest;
	dest.Now();
	dest += tv;
	ts.tv_sec = dest.get_sec();
	ts.tv_nsec = dest.get_usec() * 1000;
	int ret = LNERR_UNKNOW;
	if(pthread_mutex_lock(&mutex_) == 0) {
		if(signal_) {
			signal_ = false;
			ret = LNERR_OK;
		} else if(pthread_cond_timedwait(&cond_, &mutex_, &ts) == 0) {
			signal_ = false;
			ret = LNERR_OK;
		} else if(errno == ETIMEDOUT)
			ret = LNERR_TIMEOUT;
		pthread_mutex_unlock(&mutex_);
	}
	return ret;
#endif
}
