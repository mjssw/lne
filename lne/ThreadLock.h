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

#ifndef LNE_THREADLOCK_H
#define LNE_THREADLOCK_H

#include "config.h"

LNE_NAMESPACE_BEGIN

class LNE_Export ThreadLock
{
public:
	ThreadLock(bool fast_mode = false);
	~ThreadLock();
	void Lock(void);
	void Unlock(void);

private:
	ThreadLock(const ThreadLock &);
	ThreadLock &operator=(const ThreadLock &);
#if !defined(LNE_WIN32)
	static int pthread_mutex_spinlock(pthread_mutex_t *mutex, int spin_count);
#endif

#if defined(LNE_WIN32)
	CRITICAL_SECTION lock_;
#else
	LNE_UINT spin_count_;
	pthread_mutex_t lock_;
#endif
};

LNE_INLINE
ThreadLock::ThreadLock(bool fast_mode)
{
#if defined(LNE_WIN32)
	InitializeCriticalSectionAndSpinCount(&lock_, fast_mode ? 4000 : 0);
#else
	pthread_mutex_init(&lock_, NULL);
	spin_count_ = fast_mode ? 4000 : 0;
#endif
}

LNE_INLINE
ThreadLock::~ThreadLock(void)
{
#if defined(LNE_WIN32)
	DeleteCriticalSection(&lock_);
#else
	pthread_mutex_destroy(&lock_);
#endif
}

LNE_INLINE void
ThreadLock::Lock(void)
{
#if defined(LNE_WIN32)
	EnterCriticalSection(&lock_);
#else
	if(spin_count_ > 0)
		pthread_mutex_spinlock(&lock_, spin_count_);
	else
		pthread_mutex_lock(&lock_);
#endif
}

LNE_INLINE void
ThreadLock::Unlock(void)
{
#if defined(LNE_WIN32)
	LeaveCriticalSection(&lock_);
#else
	pthread_mutex_unlock(&lock_);
#endif
}

LNE_NAMESPACE_END

#endif
