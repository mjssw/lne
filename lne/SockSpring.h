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

#ifndef LNE_SOCKSPRING_H
#define LNE_SOCKSPRING_H

#include "SockPad.h"
#include "SockEventer.h"
#include "SockObject.h"

LNE_NAMESPACE_BEGIN

class SockSpring;
class SockSpringFactory;

class LNE_Export SockSpringHandler: public Abstract
{
public:
	virtual void HandleClient(SockSpring *spring, SockPad client) = 0;
	virtual void HandleShutdown(SockSpring *spring) = 0;
};

class LNE_Export SockSpring: public SockEventer, public SockPoolable
{
	friend class SockSpringFactory;
#if defined(LNE_WIN32)
	typedef struct : public IOCP_OVERLAPPED {
		SOCKET child;
		char address[sizeof(sockaddr_in6)];
	}	IOCP_OVERLAPPED_ACCEPT;
#endif
public:
	void *get_context(void);

	// WARNING: only used for LNE
	SockSpringHandler *get_handler(void);

protected:
	bool IdleTimeout(void);
	void HandleRead(void);
	bool HandleBind(SockPoller *poller);
	void HandleTerminate(void);

private:
	SockSpring(SockFactory *factory);
	~SockSpring(void);
	void Clean(void);

	SockSpringHandler *handler_;
	void *context_;
	SockPad skpad_;
	SockPoller *poller_;
#if defined(LNE_WIN32)
	IOCP_OVERLAPPED_ACCEPT iocp_data_;
#elif defined(LNE_LINUX)
	struct epoll_event epoll_data_;
#endif
};

class LNE_Export SockSpringFactory : public SockFactory
{
	friend class SockSpring;
public:
	static SockSpringFactory *NewInstance(LNE_UINT limit_factroy_cache = SockFactory::DEFAULT_LIMIT_CACHE);
	SockSpring *Alloc(SockPad skpad, SockSpringHandler *handler, void *context);

private:
	SockSpringFactory(LNE_UINT limit_factroy_cache);
	~SockSpringFactory(void);
	void PushObject(SockPoolable *object);
};

#include "SockSpring.inl"

LNE_NAMESPACE_END

#endif
