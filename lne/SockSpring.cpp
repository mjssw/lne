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

#include "SockSpring.h"

LNE_NAMESPACE_USING

SockSpring::SockSpring(SockBasePool *pool)
	: SockPoolable(pool), shutdown_lock_(true)
{
	shutdown_already_ = false;
#if defined(LNE_WIN32)
	memset(&iocp_data_, 0, sizeof(iocp_data_));
	iocp_data_.type = IOCP_READ;
	iocp_data_.owner = this;
	iocp_data_.child = INVALID_SOCKET;
#elif defined(LNE_LINUX)
	memset(&epoll_data_, 0, sizeof(epoll_data_));
	epoll_data_.events = EPOLLIN;
	epoll_data_.data.ptr = static_cast<SockEventer *>(this);
#endif
}

SockSpring::~SockSpring(void)
{
}

void SockSpring::Clean(void)
{
#if defined(LNE_WIN32)
	if(iocp_data_.child != INVALID_SOCKET) {
		closesocket(iocp_data_.child);
		iocp_data_.child = INVALID_SOCKET;
	}
#endif
	if(skpad_)
		skpad_.Close();
	set_poller(NULL);
	shutdown_already_ = false;
}

void SockSpring::Shutdown(void)
{
	shutdown_lock_.Lock();
	if(!shutdown_already_)
		__Shutdown();
	shutdown_lock_.Unlock();
	get_poller()->UnBind(this);
}

void SockSpring::__Shutdown(void)
{
	skpad_.Close();
	shutdown_already_ = true;
}

bool SockSpring::IdleTimeout(void)
{
	return false;
}

bool SockSpring::HandleBind(SockPoller *poller)
{
	bool result = false;
#if defined(LNE_WIN32)
	iocp_data_.child = socket(skpad_.get_family(), SOCK_STREAM, IPPROTO_TCP);
	if(iocp_data_.child != INVALID_SOCKET) {
#else
	int flags = fcntl(skpad_.get_socket(), F_GETFL);
	if(flags >= 0 && fcntl(skpad_.get_socket(), F_SETFL, flags | O_NONBLOCK) == 0) {
#endif
		set_poller(poller);
#if defined(LNE_WIN32)
		if(CreateIoCompletionPort(reinterpret_cast<HANDLE>(skpad_.get_socket()), get_poller()->Handle(), static_cast<ULONG_PTR>(skpad_.get_socket()), 0) != NULL) {
			DWORD bytes;
			if(AcceptEx(skpad_.get_socket(), iocp_data_.child, iocp_data_.address, 0, 0, sizeof(iocp_data_.address), &bytes, &iocp_data_)
					|| WSAGetLastError() == ERROR_IO_PENDING)
				result = true;
		}
#elif defined(LNE_LINUX)
		if(epoll_ctl(get_poller()->Handle(), EPOLL_CTL_ADD, skpad_.get_socket(), &epoll_data_) == 0)
			result = true;
#elif defined(LNE_FREEBSD)
		struct kevent kev[1];
		EV_SET(&kev[0], skpad_.get_socket(), EVFILT_READ, EV_ADD, 0, 0, static_cast<SockEventer *>(this));
		if(kevent(get_poller()->Handle(), kev, 1, NULL, 0, NULL) == 0)
			result = true;
#endif
	}
	if(!result)
		Clean();
	return result;
}

void SockSpring::HandleRead(void)
{
	AddRef();
	__HandleRead();
	Release();
}

void SockSpring::__HandleRead(void)
{
	bool to_handle = true;
	shutdown_lock_.Lock();
	if(shutdown_already_)
		to_handle = false;
	shutdown_lock_.Unlock();
	if(!to_handle)
		return;
	SockPad skpad;
#if defined(LNE_WIN32)
	SOCKET listen_socket = skpad_.get_socket();
	setsockopt(iocp_data_.child, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&listen_socket, sizeof(listen_socket));
	skpad.Attach(skpad_.get_family(), iocp_data_.child);
	iocp_data_.child = socket(skpad_.get_family(), SOCK_STREAM, IPPROTO_TCP);
	if(iocp_data_.child != INVALID_SOCKET) {
		DWORD bytes;
		if(!AcceptEx(skpad_.get_socket(), iocp_data_.child, &iocp_data_.address, 0, 0, sizeof(iocp_data_.address), &bytes, &iocp_data_)
				&& WSAGetLastError() != ERROR_IO_PENDING) {
			closesocket(iocp_data_.child);
			iocp_data_.child = INVALID_SOCKET;
		}
	}
	handler_->HandleClient(this, skpad);
#else
	SOCKET sock;
	do {
		shutdown_lock_.Lock();
		if(skpad_)
			sock = accept(skpad_.get_socket(), NULL, NULL);
		else {
			sock = SOCKET_ERROR;
			errno = EBADF;
		}
		shutdown_lock_.Unlock();
		if(sock != SOCKET_ERROR) {
			skpad.Attach(skpad_.get_family(), sock);
			handler_->HandleClient(this, skpad);
		}
	} while(sock != SOCKET_ERROR || errno == EINTR);
#endif
}

void SockSpring::HandleTerminate(void)
{
	__Shutdown();
	handler_->HandleTerminate(this);
	Release();
}

SockSpringPool::~SockSpringPool(void)
{
}

SockSpringPool *SockSpringPool::NewInstance(LNE_UINT limit_cache)
{
	LNE_ASSERT_RETURN(limit_cache > 0, NULL);
	SockSpringPool *result = NULL;
	try {
		result = new SockSpringPool(limit_cache);
	} catch(std::bad_alloc) {
	}
	return result;
}

SockSpring *SockSpringPool::Alloc(SockPad skpad, SockSpringHandler *handler, void *context)
{
	SockSpring *result = dynamic_cast<SockSpring *>(PopObject());
	if(result == NULL) {
		try {
			result = new SockSpring(this);
		} catch(std::bad_alloc) {
			result = NULL;
		}
	}
	if(result) {
		result->skpad_ = skpad;
		result->handler_ = handler;
		result->context_ = context;
		AddRef();
	}
	return result;
}

void SockSpringPool::PushObject(SockPoolable *object)
{
	SockBasePool::PushObject(object);
	Release();
}
