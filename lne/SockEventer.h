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

#include "BaseObject.h"

LNE_NAMESPACE_BEGIN

class SockEventer;

class LNE_Export SockPoller: public Abstract
{
public:
	virtual POLLER Handle(void) = 0;
	virtual void Bind(SockEventer *eventer) = 0;
	virtual void UnBind(SockEventer *eventer) = 0;
};

class LNE_Export SockEventer: public Abstract
{
public:
#if defined(LNE_WIN32)
	enum {IOCP_WRITE = 0, IOCP_READ = 1, IOCP_SHUTDOWN = 2};
	typedef struct : public WSAOVERLAPPED {
		WSAOVERLAPPED overlap;
		DWORD type;
		SockEventer *owner;
	}	IOCP_OVERLAPPED;
#endif

public:
	SockEventer(void);
	virtual void HandleRead(void);
	virtual void HandleWrite(void);
	virtual void HandleShutdown(void);
	virtual bool HandleBind(SockPoller* poller) = 0;
	virtual void HandleTerminate(void) = 0;
};

#include "SockEventer.inl"

LNE_NAMESPACE_END

#endif
