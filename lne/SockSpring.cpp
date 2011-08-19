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

SockSpring::SockSpring(SockFactory *factory)
	: SockPoolable(factory)
{
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
		poller_ = poller;
#if defined(LNE_WIN32)
		if(CreateIoCompletionPort(reinterpret_cast<HANDLE>(skpad_.get_socket()), poller_->Handle(), static_cast<ULONG_PTR>(skpad_.get_socket()), 0) != NULL) {
			DWORD bytes;
			if(AcceptEx(skpad_.get_socket(), iocp_data_.child, iocp_data_.address, 0, 0, sizeof(iocp_data_.address), &bytes, &iocp_data_)
					|| WSAGetLastError() == ERROR_IO_PENDING)
				result = true;
		}
#elif defined(LNE_LINUX)
		if(epoll_ctl(poller_->Handle(), EPOLL_CTL_ADD, skpad_.get_socket(), &epoll_data_) == 0)
			result = true;
#elif defined(LNE_FREEBSD)
		struct kevent kev[1];
		EV_SET(&kev[0], skpad_.get_socket(), EVFILT_READ, EV_ADD, 0, 0, static_cast<SockEventer *>(this));
		if(kevent(poller_->Handle(), kev, 1, NULL, 0, NULL) == 0)
			result = true;
#endif
	}
	if(!result)
		Clean();
	return result;
}

void SockSpring::HandleRead(void)
{
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
		sock = accept(skpad_.get_socket(), NULL, NULL);
		if(sock != SOCKET_ERROR) {
			skpad.Attach(skpad_.get_family(), sock);
			handler_->HandleClient(this, skpad);
		}
	} while(sock != SOCKET_ERROR || errno == EINTR);
#endif
}

void SockSpring::HandleTerminate(void)
{
	handler_->HandleShutdown(this);
	Release();
}

SockSpringFactory::~SockSpringFactory(void)
{
}

SockSpringFactory *SockSpringFactory::NewInstance(LNE_UINT limit_factroy_cache)
{
	LNE_ASSERT_RETURN(limit_factroy_cache > 0, NULL);
	SockSpringFactory *result = NULL;
	try {
		result = new SockSpringFactory(limit_factroy_cache);
	} catch(std::bad_alloc) {
	}
	return result;
}

SockSpring *SockSpringFactory::Alloc(SockPad skpad, SockSpringHandler *handler, void *context)
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

void SockSpringFactory::PushObject(SockPoolable *object)
{
	SockFactory::PushObject(object);
	Release();
}
