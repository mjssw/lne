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

#include "Thread.h"

LNE_NAMESPACE_USING

Thread::Thread(Runnable *run)
{
	initialized_ = false;
	run_ = run;
}

Thread::~Thread(void)
{
	Wait();
}

Thread *Thread::NewInstance(Runnable *run)
{
	return new(std::nothrow)Thread(run);
}

void Thread::Release(void)
{
	delete this;
}

LNE_UINT Thread::Active(void)
{
	if(initialized_)
		return LNERR_REENTRY;
#if defined(LNE_WIN32)
	DWORD tid;
	handle_ = CreateThread(NULL, 0, ThreadRoutine, this, 0, &tid);
	if(handle_ == NULL) {
		return LNERR_UNKNOW;
	}
#else
	if(pthread_create(&thread_, NULL, ThreadRoutine, this) != 0) {
		return LNERR_UNKNOW;
	}
#endif
	initialized_ = true;
	return LNERR_OK;
}

LNE_UINT Thread::Wait(void)
{
	run_->Terminate();
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	if(WaitForSingleObject(handle_, INFINITE) != WAIT_OBJECT_0)
		return LNERR_UNKNOW;
	CloseHandle(handle_);
#else
	pthread_join(thread_, NULL);
#endif
	initialized_ = false;
	return LNERR_OK;
}

#if defined(LNE_WIN32)
DWORD WINAPI Thread::ThreadRoutine(LPVOID parameter)
{
	static_cast<Thread *>(parameter)->run_->Service();
	return 0;
}
#else
void *Thread::ThreadRoutine(void *parameter)
{
	static_cast<Thread *>(parameter)->run_->Service();
	return (void *)0;
}
#endif
