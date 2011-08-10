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

#ifndef LNE_SOCKPAD_H
#define LNE_SOCKPAD_H

#include "config.h"
#include "SockBase.h"
#include "DataBlock.h"
#include "ThreadLock.h"
#include "ObjectQueue_T.h"

LNE_NAMESPACE_BEGIN

class SockPad;
class SockPoller;
class DataBlockPool;

struct SockHander {
	virtual void HandleData(SockPad *client, DataBlock *block) = 0;
	virtual void HandleShutdown(SockPad *client) = 0;
};

struct SockManager {
	virtual void FreeSock(SockPad *client) = 0;
};

class LNE_Export SockPad: public SockBase
{
	friend class SockPoller;
public:
#if defined(LNE_WIN32)
	enum {IOCP_RECV = 0, IOCP_SEND = 1, IOCP_CLOSE = 2};
	typedef struct {
		WSAOVERLAPPED overlap;
		DWORD type;
		SockPad *owner;
	}	IOCP_OVERLAPPED;
#endif

public:
	void Send(DataBlock *block);
	void Send(DataBlock *blocks[], LNE_UINT count);
	void Shutdown(void);
	SockHander *get_hander(void);
	void *get_context(void);

private:
	SockPad(SockManager *manager, LNE_UINT limit_cache);
	~SockPad(void);
	LNE_UINT Apply(void);
	void Clean(void);
	void __Shutdown(void);
	void HandleSend(void);
	void __HandleSend(void);
	void HandleRecv(DataBlockPool *pool);
	void __HandleRecv(DataBlockPool *pool);
	void HandleShutdown(void);
	void __HandleShutdown(void);
	void EnterThreadSafe(void);
	void LeaveThreadSafe(void);

	SockManager *manager_;
	LNE_UINT limit_cache_;
	SockHander *hander_;
	void *context_;
	// for thread safe
	LNE_UINT thread_count_;
	ThreadLock thread_lock_;
	// for send
	struct {
		bool ready;
		bool already;
		DataBlock *cache_buf;
		LNE_UINT cache_len;
	} send_state_;
	ObjectQueue<DataBlock *> send_blocks_;
	ThreadLock send_lock_;
	// for recv
	struct {
		bool ready;
		bool already;
	} recv_state_;
	ThreadLock recv_lock_;
	// for shutdown
	struct {
		bool query;
		bool invoke;
		bool already;
	} shutdown_state_;
	ThreadLock shutdown_lock_;
#if defined(LNE_WIN32)
	HANDLE poller_;
	struct {
		LNE_INT count;
		WSABUF buffer;
		IOCP_OVERLAPPED overlap[3];
	} iocp_data_;
	ThreadLock iocp_lock_;
#elif defined(LNE_LINUX)
	int poller_;
	struct epoll_event epoll_data_;
#elif defined(LNE_FREEBSD)
	int poller_;
	struct {
		LNE_UINT num_eof;
	} kevent_data_;
	ThreadLock kevent_lock_;
#endif
};

#include "SockPad.inl"

LNE_NAMESPACE_END

#endif
