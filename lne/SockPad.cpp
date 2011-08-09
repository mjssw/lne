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

#include "SockPad.h"
#include "DataBlockPool.h"

LNE_NAMESPACE_USING

SockPad::SockPad(SockManager *manager, LNE_UINT limit_cache)
	: manager_(manager), limit_cache_(limit_cache), thread_lock_(true), send_lock_(true), recv_lock_(true), shutdown_lock_(true)
{
	hander_ = NULL;
	context_ = NULL;
	thread_count_ = 0;
	memset(&send_state_, 0, sizeof(send_state_));
	memset(&recv_state_, 0, sizeof(recv_state_));
	memset(&shutdown_state_, 0, sizeof(shutdown_state_));
#if defined(LNE_WIN32)
	poller_ = NULL;
	memset(&iocp_data_, 0, sizeof(iocp_data_));
	iocp_data_.overlap[IOCP_RECV].type = IOCP_RECV;
	iocp_data_.overlap[IOCP_RECV].owner = this;
	iocp_data_.overlap[IOCP_SEND].type = IOCP_SEND;
	iocp_data_.overlap[IOCP_SEND].owner = this;
	iocp_data_.overlap[IOCP_CLOSE].type = IOCP_CLOSE;
	iocp_data_.overlap[IOCP_CLOSE].owner = this;
#elif defined(LNE_LINUX)
	poller_ = -1;
	memset(&epoll_data_, 0, sizeof(epoll_data_));
	epoll_data_.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLET;
	epoll_data_.data.ptr = this;
#elif defined(LNE_FREEBSD)
	poller_ = -1;
	memset(&kevent_data_, 0, sizeof(kevent_data_));
#endif
}

SockPad::~SockPad(void)
{
}

LNE_UINT SockPad::Apply(void)
{
	LNE_UINT result = LNERR_UNKNOW;
#if defined(LNE_WIN32)
	unsigned long value = 1;
	if(ioctlsocket(socket_, FIONBIO, &value) == 0) {
#else
	int flags = fcntl(socket_, F_GETFL);
	if(flags >= 0 && fcntl(socket_, F_SETFL, flags | O_NONBLOCK) == 0) {
#endif
#if defined(LNE_WIN32)
		if(CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_), poller_, static_cast<ULONG_PTR>(socket_), 0) != NULL) {
			iocp_lock_.Lock();
			DWORD bytes, flags = 0;
			int rc = WSARecv(socket_, &iocp_data_.buffer, 1, &bytes, &flags, reinterpret_cast<LPWSAOVERLAPPED>(&iocp_data_.overlap[IOCP_RECV]), NULL);
			if(rc != SOCKET_ERROR || WSAGetLastError() == ERROR_IO_PENDING) {
				++iocp_data_.count;
				result = LNERR_OK;
			}
			iocp_lock_.Unlock();
		}
#elif defined(LNE_LINUX)
		if(epoll_ctl(poller_, EPOLL_CTL_ADD, socket_, &epoll_data_) == 0)
			result = LNERR_OK;
#elif defined(LNE_FREEBSD)
		struct kevent kev[2];
		EV_SET(&kev[0], socket_, EVFILT_READ, EV_ADD | EV_DISABLE | EV_CLEAR, 0, 0, this);
		EV_SET(&kev[1], socket_, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_CLEAR, 0, 0, this);
		if(kevent(poller_, kev, 2, NULL, 0, NULL) == 0) {
			kevent_data_.num_eof = 1;
			EV_SET(&kev[0], socket_, EVFILT_READ, EV_ENABLE, 0, 0, this);
			EV_SET(&kev[1], socket_, EVFILT_WRITE, EV_ENABLE, 0, 0, this);
			kevent(poller_, kev, 2, NULL, 0, NULL);
			result = LNERR_OK;
		}
#endif
	}
	if(result != LNERR_OK)
		Clean();
	return result;
}

void SockPad::Clean(void)
{
	if(socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
	if(send_state_.cache_buf)
		send_state_.cache_buf->Release();
	while(send_blocks_.Extract(send_state_.cache_buf) == LNERR_OK)
		send_state_.cache_buf->Release();
	hander_ = NULL;
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

void SockPad::Send(DataBlock *block)
{
	LNE_ASSERT2(block != NULL && !block->IsEmpty());
	// ignore when shutdown
	shutdown_lock_.Lock();
	if(shutdown_state_.already || shutdown_state_.query) {
		block->Release();
		block = NULL;
	}
	shutdown_lock_.Unlock();
	if(block == NULL)
		return;
	// append to queue
	send_lock_.Lock();
	bool query_shutdown = false;
	if(send_blocks_.Append(block) != LNERR_OK) {
		block->Release();
		block = NULL;
		query_shutdown = true;
	} else if(send_blocks_.get_count() > limit_cache_)
		query_shutdown = true;
	if(query_shutdown) {
		shutdown_lock_.Lock();
		shutdown_state_.query = true;
		shutdown_lock_.Unlock();
	}
	send_lock_.Unlock();
	if(block)
		__HandleSend();
}

void SockPad::Shutdown(void)
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

void SockPad::__Shutdown(void)
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
		if(PostQueuedCompletionStatus(poller_, 0, static_cast<ULONG_PTR>(socket_), reinterpret_cast<LPWSAOVERLAPPED>(&iocp_data_.overlap[IOCP_CLOSE])))
			--iocp_data_.count;
	}
	iocp_lock_.Unlock();
#endif
}

void SockPad::HandleSend(void)
{
	EnterThreadSafe();
#if defined(LNE_WIN32)
	iocp_lock_.Lock();
	--iocp_data_.count;
	iocp_lock_.Unlock();
#endif
	__HandleSend();
	LeaveThreadSafe();
}

void SockPad::__HandleSend(void)
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
		if(send_state_.cache_buf == NULL) {
			send_lock_.Lock();
			if(send_blocks_.Extract(block) != LNERR_OK) {
				send_state_.ready = true;
				continue_send = false;
				block_empty = true;
			} else {
				send_state_.cache_buf = block;
				send_state_.cache_len = 0;
			}
			send_lock_.Unlock();
		}
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
				send_state_.cache_len += len;
				if(send_state_.cache_len == send_state_.cache_buf->get_size()) {
					send_state_.cache_buf->Release();
					send_state_.cache_buf = NULL;
					send_state_.cache_len = 0;
				}
			} else {
				continue_send = false;
#if defined(LNE_WIN32)
				if(WSAGetLastError() != WSAEWOULDBLOCK)
					to_shutdown = true;
				else {
					DWORD bytes;
					iocp_lock_.Lock();
					int rc = WSASend(socket_, &iocp_data_.buffer, 1, &bytes, 0, reinterpret_cast<LPWSAOVERLAPPED>(&iocp_data_.overlap[IOCP_SEND]), NULL);
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

void SockPad::HandleRecv(DataBlockPool *pool)
{
	EnterThreadSafe();
#if defined(LNE_WIN32)
	iocp_lock_.Lock();
	--iocp_data_.count;
	iocp_lock_.Unlock();
#endif
	__HandleRecv(pool);
	LeaveThreadSafe();
}

void SockPad::__HandleRecv(DataBlockPool *pool)
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
		block = pool->Alloc();
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
				hander_->HandleData(this, block);
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
				int rc = WSARecv(socket_, &iocp_data_.buffer, 1, &bytes, &flags, reinterpret_cast<LPWSAOVERLAPPED>(&iocp_data_.overlap[IOCP_RECV]), NULL);
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

void SockPad::HandleShutdown(void)
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

void SockPad::__HandleShutdown(void)
{
	shutdown_lock_.Lock();
	shutdown_state_.already = true;
	shutdown_lock_.Unlock();
}

void SockPad::EnterThreadSafe(void)
{
	thread_lock_.Lock();
	++thread_count_;
	thread_lock_.Unlock();
}

void SockPad::LeaveThreadSafe(void)
{
	LNE_UINT num_flag = 0;
	thread_lock_.Lock();
	--thread_count_;
	if(thread_count_ == 0)
		++num_flag;
	thread_lock_.Unlock();
	shutdown_lock_.Lock();
	if(shutdown_state_.already)
		++num_flag;
	shutdown_lock_.Unlock();
	// process shutdown
	if(num_flag == 2) {
		hander_->HandleShutdown(this);
		manager_->FreeSock(this);
	}
}

