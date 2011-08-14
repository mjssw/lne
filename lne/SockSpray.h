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

#ifndef LNE_SOCKSPRAY_H
#define LNE_SOCKSPRAY_H

#include "DataBlock.h"
#include "SockPad.h"
#include "SockStream.h"
#include "SockEventer.h"
#include "SockObject.h"
#include "ObjectQueue_T.h"

LNE_NAMESPACE_BEGIN

class DataBlockPool;
class SockPoller;
class SockSpray;
class SockSprayFactory;

class LNE_Export SockSprayHander: public Abstract
{
public:
	virtual void HandleData(SockSpray *client, DataBlock *block) = 0;
	virtual void HandleShutdown(SockSpray *client) = 0;
};

class LNE_Export SockSpray: public SockPoolable, public SockStream, public SockEventer
{
	friend class SockSprayFactory;
public:
	bool Bind(POLLER poller);
	void Send(DataBlock *block);
	void Send(DataBlock *blocks[], LNE_UINT count);
	void Shutdown(void);
	SockSprayHander *get_hander(void);
	void *get_context(void);

private:
	SockSpray(SockFactory *factory);
	~SockSpray(void);
	void Clean(void);
	void __Shutdown(void);
	void HandleWrite(void);
	void __HandleWrite(void);
	void HandleRead(void);
	void __HandleRead(void);
	void HandleShutdown(void);
	void __HandleShutdown(void);
	void EnterThreadSafe(void);
	void LeaveThreadSafe(void);

	DataBlockPool *pool_;
	LNE_UINT limit_write_cache_;
	SockSprayHander *hander_;
	void *context_;
	ThreadLock lock_;
	LNE_UINT thread_count_;
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
	POLLER poller_;
#if defined(LNE_WIN32)
	struct {
		LNE_INT count;
		WSABUF buffer;
		IOCP_OVERLAPPED overlap[3];
	} iocp_data_;
	ThreadLock iocp_lock_;
#elif defined(LNE_LINUX)
	struct epoll_event epoll_data_;
#elif defined(LNE_FREEBSD)
	struct {
		LNE_UINT num_eof;
	} kevent_data_;
	ThreadLock kevent_lock_;
#endif
};

class LNE_Export SockSprayFactory : public SockFactory
{
	friend class SockSpray;
public:
	SockSprayFactory(DataBlockPool *pool, LNE_UINT limit_write_cache = 128);
	SockSpray *Alloc(SockPad sock, SockSprayHander *hander, void *context);

private:
	DataBlockPool *pool_;
	LNE_UINT limit_write_cache_;
};

#include "SockSpray.inl"

LNE_NAMESPACE_END

#endif
