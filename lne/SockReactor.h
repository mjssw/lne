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

#ifndef LNE_SOCKREACTOR_H
#define LNE_SOCKREACTOR_H

#include "SockEventer.h"
#include "ExtendObject.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockReactor: public RefObject, public Available, public SockPoller
{
	static const LNE_UINT EXIT_CHECK_INTERVAL = 10;           // second
	static const LNE_UINT DEFAULT_IDLE_TIMEOUT = 5 * 60 * 60; // second
	static const LNE_UINT DEFAULT_IDLE_CHECK_INTERVAL = 5;    // second
	static const LNE_UINT DEFAULT_IDLE_CHECK_ITEMS = 15;
public:
	static SockReactor *NewInstance(LNE_UINT workers, LNE_UINT idle_timeout = DEFAULT_IDLE_TIMEOUT, LNE_UINT exit_check_interval = EXIT_CHECK_INTERVAL);

	POLLER Handle(void);
	void Bind(SockEventer *eventer);
	void UnBind(SockEventer *eventer);

private:
	SockReactor(LNE_UINT workers, LNE_UINT idle_timeout, LNE_UINT exit_check_interval);
	~SockReactor(void);
	void Timer(void);
	void Service(void);
	void ObjectDestroy(void);
#if defined(LNE_WIN32)
	static DWORD WINAPI ThreadTimer(LPVOID parameter);
	static DWORD WINAPI ThreadService(LPVOID parameter);
#else
	static void *ThreadTimer(void *parameter);
	static void *ThreadService(void *parameter);
#endif

	POLLER poller_;
	LNE_UINT idle_timeout_;
	LNE_UINT exit_check_interval_;
	ThreadLock eventer_lock_;
	SockEventer *eventer_circle_;
#if defined(LNE_WIN32)
	HANDLE *threads_;
#else
	pthread_t *threads_;
#endif
	bool exit_request_;
	LNE_UINT thread_workers_;
};

LNE_NAMESPACE_END

#endif
