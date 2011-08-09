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

#include "config.h"
#include "TimeValue.h"

LNE_NAMESPACE_BEGIN

class LNE_Export ThreadCondition
{
public:
	ThreadCondition(void);
	~ThreadCondition(void);

	operator bool(void) const;
	LNE_UINT Wait(void);
	LNE_UINT Wait(const TimeValue &tv);;
	LNE_UINT Signal(void);

private:
	ThreadCondition(const ThreadCondition &);
	void operator= (const ThreadCondition &);

	bool initialized_;
#if defined(LNE_WIN32)
	HANDLE event_;
#else
	bool signal_;
	pthread_mutex_t mutex_;
	pthread_cond_t cond_;
#endif
};

#include "ThreadCondition.inl"

LNE_NAMESPACE_END

#endif
