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

#ifndef LNE_THREADMUTEX_H
#define LNE_THREADMUTEX_H

#include "BaseObject.h"
#include "TimeValue.h"

LNE_NAMESPACE_BEGIN

class LNE_Export ThreadMutex: public Available, public NonCopyable
{
public:
	ThreadMutex(void);
	~ThreadMutex(void);

	LNE_UINT TryAcquire(void);
	LNE_UINT Acquire(void);
	LNE_UINT Acquire(const TimeValue &tv);;
	LNE_UINT Release(void);

private:
#if defined(LNE_WIN32)
	HANDLE mutex_;
#else
	pthread_mutex_t mutex_;
#endif
};

#include "ThreadMutex.inl"

LNE_NAMESPACE_END

#endif
