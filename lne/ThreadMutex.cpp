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

#include "ThreadMutex.h"

LNE_NAMESPACE_USING

ThreadMutex::ThreadMutex(void)
{
#if defined(LNE_WIN32)
	mutex_ = CreateMutex(NULL, FALSE, NULL);
	set_available(mutex_ != NULL);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex_, &attr);
	pthread_mutexattr_destroy(&attr);
	set_available(true);
#endif
}

ThreadMutex::~ThreadMutex(void)
{
	if(IsAvailable())
#if defined(LNE_WIN32)
		CloseHandle(mutex_);
#else
		pthread_mutex_destroy(&mutex_);
#endif
}

LNE_UINT ThreadMutex::Acquire(const TimeValue &tv)
{
	if(!IsAvailable())
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	DWORD ret = WaitForSingleObject(mutex_, (DWORD)tv.ToMillisecond());
	if(ret == WAIT_OBJECT_0)
		return LNERR_OK;
	if(ret == WAIT_TIMEOUT)
		return LNERR_TIMEOUT;
#else
	struct timespec ts;
	TimeValue dest;
	dest.Now();
	dest += tv;
	ts.tv_sec = dest.sec();
	ts.tv_nsec = dest.usec() * 1000;
	if(pthread_mutex_timedlock(&mutex_, &ts) == 0)
		return LNERR_OK;
	if(errno == ETIMEDOUT)
		return LNERR_TIMEOUT;
#endif
	return LNERR_UNKNOW;
}
