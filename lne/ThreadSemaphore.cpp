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

#include "ThreadSemaphore.h"

LNE_NAMESPACE_USING

ThreadSemaphore::ThreadSemaphore(LNE_UINT count, LNE_UINT max)
{
#if defined(LNE_WIN32)
	sem_ = CreateSemaphore(NULL, count, max, NULL);
	set_available(sem_ != NULL);
#else
	set_available(sem_init(&sem_, 0, count));
#endif
}

ThreadSemaphore::~ThreadSemaphore(void)
{
	if(IsAvailable())
#if defined(LNE_WIN32)
		CloseHandle(sem_);
#else
		sem_destroy(&sem_);
#endif
}

LNE_UINT ThreadSemaphore::Acquire(const TimeValue &tv)
{
	if(!IsAvailable())
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	DWORD ret = WaitForSingleObject(sem_, (DWORD)tv.ToMillisecond());
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
	if(sem_timedwait(&sem_, &ts) == 0)
		return LNERR_OK;
	if(errno == ETIMEDOUT)
		return LNERR_TIMEOUT;
#endif
	return LNERR_UNKNOW;
}
