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

#include "config.h"
#include "SockSpray.h"
#include "SockWaves.h"
#include "DataBlockPool.h"
#include "ObjectStack_T.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockPoller: private SockManager
{
public:
	static const LNE_UINT DEFAULT_LIMIT_CACHE = 128;

	static SockPoller *NewInstance(DataBlockPool *pool, LNE_UINT workers, LNE_UINT limit_cache = DEFAULT_LIMIT_CACHE);
	void Release(void);

	LNE_UINT Managed(SockPad& sock, SockHander *hander, void *context = NULL);

private:
	SockPoller(DataBlockPool *pool, LNE_UINT workers, LNE_UINT limit_cache);
	~SockPoller(void);
	void Service(void);
	void FreeSock(SockSpray *client);

	DataBlockPool *pool_;
	bool initialized_;
	bool exit_request_;
	ThreadLock lock_;
	LNE_UINT reference_count_;
	LNE_UINT thread_workers_;
	LNE_UINT limit_cache_;
	ObjectStack<SockSpray *, 1000> clients_free_;
#if defined(LNE_WIN32)
	HANDLE poller_;
	HANDLE *threads_;
	static DWORD WINAPI ThreadRoutine(LPVOID parameter);
#else
	int poller_;
	pthread_t *threads_;
	static void *ThreadRoutine(void *parameter);
#endif
};

#include "SockPoller.inl"

LNE_NAMESPACE_END

#endif
