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

#include "SockSpray.h"
#include "DataBlockPool.h"

LNE_NAMESPACE_USING

SockSpray::SockSpray(SockFactory *factory)
	: SockPoolable(factory),
	  pool_(dynamic_cast<SockSprayFactory *>(factory)->pool_),
	  limit_write_cache_(dynamic_cast<SockSprayFactory *>(factory)->limit_write_cache_),
	  lock_(true), send_lock_(true), recv_lock_(true), shutdown_lock_(true)
{
	handler_ = NULL;
	context_ = NULL;
	thread_count_ = 0;
	memset(&send_state_, 0, sizeof(send_state_));
	memset(&recv_state_, 0, sizeof(recv_state_));
	memset(&shutdown_state_, 0, sizeof(shutdown_state_));
	poller_ = INVALID_POLLER;
#if defined(LNE_WIN32)
	memset(&iocp_data_, 0, sizeof(iocp_data_));
	iocp_data_.overlap[IOCP_READ].type = IOCP_READ;
	iocp_data_.overlap[IOCP_READ].owner = this;
	iocp_data_.overlap[IOCP_WRITE].type = IOCP_WRITE;
	iocp_data_.overlap[IOCP_WRITE].owner = this;
	iocp_data_.overlap[IOCP_SHUTDOWN].type = IOCP_SHUTDOWN;
	iocp_data_.overlap[IOCP_SHUTDOWN].owner = this;
#elif defined(LNE_LINUX)
	memset(&epoll_data_, 0, sizeof(epoll_data_));
	epoll_data_.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLET;
	epoll_data_.data.ptr = static_cast<SockEventer *>(this);
#elif defined(LNE_FREEBSD)
	memset(&kevent_data_, 0, sizeof(kevent_data_));
#endif
	pool_->AddRef();
}

SockSpray::~SockSpray(void)
{
	pool_->Release();
}

bool SockSpray::HandleBind(SockPoller *poller)
{
	bool result = false;
#if defined(LNE_WIN32)
	unsigned long value = 1;
	if(ioctlsocket(socket_, FIONBIO, &value) == 0) {
#else
	int flags = fcntl(socket_, F_GETFL);
	if(flags >= 0 && fcntl(socket_, F_SETFL, flags | O_NONBLOCK) == 0) {
#endif
		poller_ = poller;
#if defined(LNE_WIN32)
		if(CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_), poller_->Handle(), static_cast<ULONG_PTR>(socket_), 0) != NULL) {
			iocp_lock_.Lock();
			DWORD bytes, flags = 0;
			int rc = WSARecv(socket_, &iocp_data_.buffer, 1, &bytes, &flags, &iocp_data_.overlap[IOCP_READ], NULL);
			if(rc != SOCKET_ERROR || WSAGetLastError() == ERROR_IO_PENDING) {
				++iocp_data_.count;
				result = true;
			}
			iocp_lock_.Unlock();
		}
#elif defined(LNE_LINUX)
		if(epoll_ctl(poller_->Handle(), EPOLL_CTL_ADD, socket_, &epoll_data_) == 0)
			result = true;
#elif defined(LNE_FREEBSD)
		struct kevent kev[2];
		EV_SET(&kev[0], socket_, EVFILT_READ, EV_ADD | EV_DISABLE | EV_CLEAR, 0, 0, static_cast<SockEventer *>(this));
		EV_SET(&kev[1], socket_, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_CLEAR, 0, 0, static_cast<SockEventer *>(this));
		if(kevent(poller_->Handle(), kev, 2, NULL, 0, NULL) == 0) {
			kevent_data_.num_eof = 1;
			EV_SET(&kev[0], socket_, EVFILT_READ, EV_ENABLE, 0, 0, static_cast<SockEventer *>(this));
			EV_SET(&kev[1], socket_, EVFILT_WRITE, EV_ENABLE, 0, 0, static_cast<SockEventer *>(this));
			kevent(poller_->Handle(), kev, 2, NULL, 0, NULL);
			result = true;
		}
#endif
	}
	if(!result)
		Clean();
	return result;
}

void SockSpray::HandleTerminate(void)
{
	shutdown_lock_.Lock();
	shutdown_state_.already = true;
	shutdown_lock_.Unlock();
	handler_->HandleShutdown(this);
	Release();
}

void SockSpray::Clean(void)
{
	if(socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
	if(send_state_.cache_buf)
		send_state_.cache_buf->Release();
	while(send_blocks_.Extract(send_state_.cache_buf) == LNERR_OK)
		send_state_.cache_buf->Release();
	poller_ = INVALID_POLLER;
	handler_ = NULL;
	context_ = NULL;
	thread_count_ = 0;
	memset(&send_state_, 0, sizeof(send_state_));
	memset(&recv_state_, 0, sizeof(recv_state_));
	memset(&shutdown_state_, 0, sizeof(shutdown_state_));
#if defined(LNE_WIN32)
	iocp_data_.count = 0;
#elif defined(LNE_FREEBSD)
	kevent_data_.num_eof = 0;
#endif
}

void SockSpray::Send(DataBlock *block)
{
	LNE_ASSERT_RETURN_VOID(block != NULL && !block->IsEmpty());
	bool to_handle = true;
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already || shutdown_state_.query)
		to_handle = false;
	shutdown_lock_.Unlock();
	if(!to_handle)
		return;
	// append to queue
	send_lock_.Lock();
	block->AddRef();
	if(send_blocks_.Append(block) != LNERR_OK) {
		block->Release();
		to_handle = false;
	} else if(send_blocks_.get_count() > limit_write_cache_)
		to_handle = false;
	if(!to_handle) {
		shutdown_lock_.Lock();
		shutdown_state_.query = true;
		shutdown_lock_.Unlock();
	}
	send_lock_.Unlock();
	if(to_handle)
		__HandleWrite();
}

void SockSpray::Send(DataBlock *blocks[], LNE_UINT count)
{
	LNE_ASSERT_RETURN_VOID(blocks != NULL && count > 0);
	bool to_handle = true;
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already || shutdown_state_.query)
		to_handle = false;
	shutdown_lock_.Unlock();
	if(!to_handle)
		return;
	// append to queue
	DataBlock *block;
	send_lock_.Lock();
	for(LNE_UINT i = 0; to_handle && i < count; ++i) {
		block = blocks[i];
		block->AddRef();
		if(send_blocks_.Append(block) != LNERR_OK) {
			block->Release();
			to_handle = false;
		} else if(send_blocks_.get_count() > limit_write_cache_)
			to_handle = false;
	}
	if(!to_handle) {
		shutdown_lock_.Lock();
		shutdown_state_.query = true;
		shutdown_lock_.Unlock();
	}
	send_lock_.Unlock();
	if(to_handle)
		__HandleWrite();
}

void SockSpray::Shutdown(void)
{
	bool to_handle = true;
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already || shutdown_state_.query)
		to_handle = false;
	else
		shutdown_state_.query = true;
	shutdown_lock_.Unlock();
	if(to_handle) {
		// send buffers must empty
		send_lock_.Lock();
		if(!send_blocks_.IsEmpty() || send_state_.cache_buf != NULL)
			to_handle = false;
		send_lock_.Unlock();
		if(to_handle) {
			shutdown_lock_.Lock();
			__Shutdown();
			shutdown_lock_.Unlock();
		}
	}
}

void SockSpray::__Shutdown(void)
{
	if(!shutdown_state_.invoke) {
		shutdown_state_.invoke = true;
#if defined(LNE_FREEBSD)
		kevent_lock_.Lock();
		kevent_data_.num_eof = 2;
		kevent_lock_.Unlock();
#endif
#if defined(LNE_WIN32)
		shutdown(socket_, SD_BOTH);
#else
		shutdown(socket_, SHUT_RDWR);
#endif
	}
#if defined(LNE_WIN32)
	iocp_lock_.Lock();
	if(iocp_data_.count == 0) {
		if(PostQueuedCompletionStatus(poller_->Handle(), 0, static_cast<ULONG_PTR>(socket_), &iocp_data_.overlap[IOCP_SHUTDOWN]))
			--iocp_data_.count;
	}
	iocp_lock_.Unlock();
#endif
}

void SockSpray::HandleWrite(void)
{
	EnterThreadSafe();
#if defined(LNE_WIN32)
	iocp_lock_.Lock();
	--iocp_data_.count;
	iocp_lock_.Unlock();
#endif
	__HandleWrite();
	LeaveThreadSafe();
}

void SockSpray::__HandleWrite(void)
{
	bool to_handle = true;
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already)
		to_handle = false;
	shutdown_lock_.Unlock();
	if(!to_handle)
		return;
	// lock send process
	send_lock_.Lock();
	if(send_state_.already) {
		to_handle = false;
		send_state_.ready = true;
	} else {
		send_state_.ready = false;
		send_state_.already = true;
	}
	send_lock_.Unlock();
	if(!to_handle)
		return;
	// process data send
	ssize_t len;
	DataBlock *block;
	bool continue_send = false, to_shutdown = false, block_empty = false;
send_data_next:
	do {
		// extract block
		send_lock_.Lock();
		if(send_state_.cache_buf == NULL) {
			if(send_blocks_.Extract(block) != LNERR_OK) {
				send_state_.ready = true;
				continue_send = false;
				block_empty = true;
			} else {
				send_state_.cache_buf = block;
				send_state_.cache_len = 0;
			}
		}
		send_lock_.Unlock();
		// send cache buffer
		if(send_state_.cache_buf) {
#if defined(LNE_WIN32)
			len = send(socket_, send_state_.cache_buf->get_buffer() + send_state_.cache_len, send_state_.cache_buf->get_size() - send_state_.cache_len, 0);
#else
			do {
				len = send(socket_, send_state_.cache_buf->get_buffer() + send_state_.cache_len, send_state_.cache_buf->get_size() - send_state_.cache_len, MSG_NOSIGNAL);
			} while(len < 0 && errno == EINTR);
#endif
			if(len > 0) {
				continue_send = true;
				send_lock_.Lock();
				send_state_.cache_len += len;
				if(send_state_.cache_len == send_state_.cache_buf->get_size()) {
					send_state_.cache_buf->Release();
					send_state_.cache_buf = NULL;
					send_state_.cache_len = 0;
				}
				send_lock_.Unlock();
			} else {
				continue_send = false;
#if defined(LNE_WIN32)
				if(WSAGetLastError() != WSAEWOULDBLOCK)
					to_shutdown = true;
				else {
					DWORD bytes;
					iocp_lock_.Lock();
					int rc = WSASend(socket_, &iocp_data_.buffer, 1, &bytes, 0, &iocp_data_.overlap[IOCP_WRITE], NULL);
					if(rc == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
						to_shutdown = true;
					else
						++ iocp_data_.count;
					iocp_lock_.Unlock();
				}
#else
				if(errno != EWOULDBLOCK)
					to_shutdown = true;
#endif
			}
		}
	} while(continue_send && !to_shutdown);
	// check shutdown
	shutdown_lock_.Lock();
	if(!shutdown_state_.already && (to_shutdown || (block_empty && shutdown_state_.query))) {
		to_shutdown = true;
		__Shutdown();
	}
	shutdown_lock_.Unlock();
	// unlock send process
	to_handle = false;
	send_lock_.Lock();
	send_state_.already = false;
	if(send_state_.ready && !to_shutdown && !block_empty)
		to_handle = true;
	send_lock_.Unlock();
	if(to_handle)
		goto send_data_next;
}

void SockSpray::HandleRead(void)
{
	EnterThreadSafe();
#if defined(LNE_WIN32)
	iocp_lock_.Lock();
	--iocp_data_.count;
	iocp_lock_.Unlock();
#endif
	__HandleRead();
	LeaveThreadSafe();
}

void SockSpray::__HandleRead(void)
{
	bool to_handle = true;
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already || shutdown_state_.query)
		to_handle = false;
	shutdown_lock_.Unlock();
	if(!to_handle)
		return;
	// lock recv process
	recv_lock_.Lock();
	if(recv_state_.already) {
		to_handle = false;
		recv_state_.ready = true;
	} else {
		recv_state_.ready = false;
		recv_state_.already = true;
	}
	recv_lock_.Unlock();
	if(!to_handle)
		return;
	ssize_t len;
	DataBlock *block;
	bool continue_recv = false, to_shutdown = false;
recv_data_next:
	do {
		block = pool_->Alloc();
		if(block == NULL)
			to_shutdown = true;
		else {
			do {
				len = recv(socket_, block->get_buffer_free(), block->get_free_size(), 0);
				if(len > 0)
					block->set_size(block->get_size() + len);
#if defined(LNE_WIN32)
			} while(len > 0 && !block->IsFull());
#else
			}
			while((len > 0 && !block->IsFull()) || (len < 0 && errno == EINTR));
#endif
			if(len > 0)
				continue_recv = true;
#if defined(LNE_WIN32)
			else if(len == 0 || WSAGetLastError() != WSAEWOULDBLOCK)
#else
			else if(len == 0 || errno != EWOULDBLOCK)
#endif
				to_shutdown = true;
			// process data
			if(!block->IsEmpty())
				handler_->HandleData(this, block);
			else
				block->Release();
			if(continue_recv) {
				shutdown_lock_.Lock();
				if(shutdown_state_.query) {
					to_shutdown = true;
					continue_recv = false;
				}
				shutdown_lock_.Unlock();
			}
#if defined(LNE_WIN32)
			if(!continue_recv && !to_shutdown) {
				DWORD bytes, flags = 0;
				iocp_lock_.Lock();
				int rc = WSARecv(socket_, &iocp_data_.buffer, 1, &bytes, &flags, &iocp_data_.overlap[IOCP_READ], NULL);
				if(rc == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
					to_shutdown = true;
				else
					++iocp_data_.count;
				iocp_lock_.Unlock();
			}
#endif
		}
	}
	while(continue_recv && !to_shutdown);
	// check shutdown
	shutdown_lock_.Lock();
	if(!shutdown_state_.already && to_shutdown)
		__Shutdown();
	shutdown_lock_.Unlock();
	// lock unlock process
	to_handle = false;
	recv_lock_.Lock();
	recv_state_.already = false;
	if(recv_state_.ready && !to_shutdown)
		to_handle = true;
	recv_lock_.Unlock();
	if(to_handle)
		goto recv_data_next;
}

void SockSpray::HandleShutdown(void)
{
#if defined(LNE_FREEBSD)
	bool to_handle = false;
	kevent_lock_.Lock();
	if(--kevent_data_.num_eof == 0)
		to_handle = true;
	kevent_lock_.Unlock();
	if(!to_handle)
		return;
#endif
	EnterThreadSafe();
	__HandleShutdown();
	LeaveThreadSafe();
}

void SockSpray::__HandleShutdown(void)
{
	shutdown_lock_.Lock();
	shutdown_state_.already = true;
	shutdown_lock_.Unlock();
}

void SockSpray::EnterThreadSafe(void)
{
	lock_.Lock();
	++thread_count_;
	lock_.Unlock();
}

void SockSpray::LeaveThreadSafe(void)
{
	LNE_UINT num_flag = 0;
	lock_.Lock();
	--thread_count_;
	if(thread_count_ == 0)
		++num_flag;
	lock_.Unlock();
	shutdown_lock_.Lock();
	if(shutdown_state_.already)
		++num_flag;
	shutdown_lock_.Unlock();
	// process shutdown
	if(num_flag == 2) {
		handler_->HandleShutdown(this);
		poller_->UnBind(this);
		Release();
	}
}

SockSprayFactory::~SockSprayFactory()
{
	pool_->Release();
}

SockSprayFactory *SockSprayFactory::NewInstance(DataBlockPool *pool, LNE_UINT limit_write_cache, LNE_UINT limit_factroy_cache)
{
	LNE_ASSERT_RETURN(pool != NULL && limit_write_cache > 0, NULL);
	SockSprayFactory *result = NULL;
	try {
		result = new SockSprayFactory(limit_factroy_cache);
		result->pool_ = pool;
		result->pool_->AddRef();
		result->limit_write_cache_ = limit_write_cache;
	} catch(std::bad_alloc) {
	}
	return result;
}

SockSpray *SockSprayFactory::Alloc(SockPad sock, SockSprayHandler *handler, void *context)
{
	SockSpray *result = dynamic_cast<SockSpray *>(PopObject());
	if(result == NULL) {
		try {
			result = new SockSpray(this);
		} catch(std::bad_alloc) {
			result = NULL;
		}
	}
	if(result) {
		result->socket_ = sock.Detach();
		result->handler_ = handler;
		result->context_ = context;
	}
	return result;
}
