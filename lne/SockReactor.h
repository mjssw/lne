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

#ifndef LNE_SOCKPOLLER_H
#define LNE_SOCKPOLLER_H

#include "ExtendObject.h"
#include "SockEventer.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockReactor: public RefObject, public Available
{
public:
	static SockReactor *NewInstance(LNE_UINT workers);

	LNE_UINT Managed(SockEventer* eventer);

private:
	SockReactor(LNE_UINT workers);
	~SockReactor(void);
	void Service(void);
	void ObjectDestroy();

	ThreadLock lock_;
	bool exit_request_;
	LNE_UINT thread_workers_;
	POLLER poller_;
#if defined(LNE_WIN32)
	HANDLE *threads_;
	static DWORD WINAPI ThreadRoutine(LPVOID parameter);
#else
	pthread_t *threads_;
	static void *ThreadRoutine(void *parameter);
#endif
};

#include "SockReactor.inl"

LNE_NAMESPACE_END

#endif
