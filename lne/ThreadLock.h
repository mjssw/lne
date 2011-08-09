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
	void operator= (const ThreadLock &);
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

#include "ThreadLock.inl"

LNE_NAMESPACE_END

#endif
