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

#ifndef LNE_THREADPOOL_H
#define LNE_THREADPOOL_H

#include "BaseObject.h"
#include "ThreadLock.h"
#include "ThreadCondition.h"

LNE_NAMESPACE_BEGIN

class ThreadPool;

class LNE_Export ThreadTask: public Abstract, public NonCopyable
{
	friend class ThreadPool;
public:
	ThreadTask(void);
	virtual void Service(void) = 0;
	virtual void Discard(void) = 0;

private:
	ThreadTask *next_;
};

class LNE_Export ThreadPool: public Available
{
public:
	static ThreadPool *NewInstance(LNE_UINT limit);
	void Release(void);

	LNE_UINT PostTask(ThreadTask *task);

private:
	ThreadPool(LNE_UINT init, LNE_UINT limit);
	~ThreadPool(void);
	void Service(void);

	bool exit_request_;
	LNE_UINT num_thread_;
	LNE_UINT run_thread_;
	LNE_UINT limit_thread_;
	LNE_UINT num_tasks_;
	ThreadTask *tasks_head_;
	ThreadTask *tasks_rear_;
	ThreadLock lock_;
	ThreadCondition condition_;
#if defined(LNE_WIN32)
	HANDLE *threads_;
	static DWORD WINAPI ThreadRoutine(LPVOID parameter);
#else
	pthread_t *threads_;
	static void *ThreadRoutine(void *parameter);
#endif
};

LNE_NAMESPACE_END

#endif
