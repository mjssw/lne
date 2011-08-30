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

#ifndef LNE_SOCKEVENTER_H
#define LNE_SOCKEVENTER_H

#include "ExtendObject.h"
#include "ObjectStack_T.h"

LNE_NAMESPACE_BEGIN

class SockEventer;
class SockEventerPool;

class LNE_Export SockPoller: public Abstract
{
public:
	virtual POLLER Handle(void) = 0;
	virtual void Bind(SockEventer *eventer) = 0;
	virtual void UnBind(SockEventer *eventer) = 0;
};

class LNE_Export SockEventer: public RefObject
{
public:
#if defined(LNE_WIN32)
	enum {IOCP_WRITE = 0, IOCP_READ, IOCP_SHUTDOWN, IOCP_ARRAY_MAX};
	typedef struct : public WSAOVERLAPPED {
		WSAOVERLAPPED overlap;
		DWORD type;
		SockEventer *owner;
	}	IOCP_OVERLAPPED;
#endif

public:
	SockEventer(SockEventerPool *pool);
	void *context();
	void set_context(void *context);

	virtual void Shutdown(void) = 0;
	virtual bool IdleTimeout(void) = 0;
	virtual void HandleRead(void);
	virtual void HandleWrite(void);
	virtual void HandleShutdown(void);
	virtual void HandleIdleTimeout(void);
	virtual bool HandleBind(SockPoller *binder) = 0;
	virtual void HandleTerminate(void) = 0;

	// WARNING: only used for LNE
	SockEventer *prev(void);
	void set_prev(SockEventer *prev);
	SockEventer *next(void);
	void set_next(SockEventer *prev);
	time_t active(void);
	void set_active(time_t active);

protected:
	SockPoller *poller(void);
	void set_poller(SockPoller *poller);
	void ObjectDestroy(void);
	virtual void Clean(void) = 0;

private:
	SockPoller *poller_;
	SockEventer *prev_;
	SockEventer *next_;
	time_t active_;
	void *context_;
	SockEventerPool *pool_;
};

class LNE_Export SockEventerPool: public RefObject
{
	friend class SockEventer;
public:
	static const LNE_UINT DEFAULT_LIMIT_CACHE = 128;

	SockEventerPool(LNE_UINT limit_cache);
	~SockEventerPool(void);

protected:
	void ObjectDestroy(void);
	virtual void PushObject(SockEventer *object);
	virtual SockEventer *PopObject(void);

private:
	LNE_UINT limit_cache_;
	ObjectStack<SockEventer *>  objects_;
};

LNE_INLINE void *
SockEventer::context(void)
{
	return context_;
}

LNE_INLINE void
SockEventer::set_context(void *context)
{
	context_ = context;
}

LNE_INLINE SockEventer *
SockEventer::prev(void)
{
	return prev_;
}

LNE_INLINE void
SockEventer::set_prev(SockEventer *prev)
{
	prev_ = prev;
}

LNE_INLINE SockEventer *
SockEventer::next(void)
{
	return next_;
}

LNE_INLINE void
SockEventer::set_next(SockEventer *next)
{
	next_ = next;
}

LNE_INLINE time_t
SockEventer::active(void)
{
	return active_;
}

LNE_INLINE void
SockEventer::set_active(time_t active)
{
	active_ = active;
}

LNE_INLINE SockPoller *
SockEventer::poller(void)
{
	return poller_;
}

LNE_INLINE void
SockEventer::set_poller(SockPoller *poller)
{
	poller_ = poller;
}

LNE_NAMESPACE_END

#endif
