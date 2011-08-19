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

#include "SockConnector.h"

LNE_NAMESPACE_USING

SockConnector::SockConnector(void)
{
	use_timeout_ = false;
}

SockConnector::~SockConnector(void)
{
}

SockConnector *SockConnector::NewInstance(const SockAddr &addr, const TimeValue *tv)
{
	try {
		SockConnector *connector = new SockConnector();
		connector->addr_ = addr;
		if(tv) {
			connector->use_timeout_ = true;
			connector->timeout_ = *tv;
		}
		return connector;
	} catch(std::bad_alloc) {
	}
	return NULL;
}

void SockConnector::Release(void)
{
	delete this;
}

LNE_UINT SockConnector::Connect(SockPad &skpad, const SockAddr &addr, const TimeValue *tv)
{
	LNE_UINT result = LNERR_UNKNOW;
	SOCKET sock = socket(addr.get_family(), SOCK_STREAM, IPPROTO_TCP);
	if(sock != INVALID_SOCKET) {
		if(tv) {
#if defined(LNE_WIN32)
			unsigned long value = 1;
			if(ioctlsocket(sock, FIONBIO, &value) == 0) {
#else
			int flags = fcntl(sock, F_GETFL);
			if(flags >= 0 && fcntl(sock, F_SETFL, flags | O_NONBLOCK) == 0) {
#endif
				connect(sock, addr.get_addr(), addr.get_size());
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET((SOCKET)sock, &fds);
				TimeValue timeout(*tv);
				if(select(static_cast<int>((SOCKET)sock + 1), NULL, &fds, NULL, static_cast<timeval *>(timeout)) < 1)
					result = LNERR_TIMEOUT;
				else {
#if defined(LNE_WIN32)
					value = 0;
					if(ioctlsocket(sock, FIONBIO, &value) == 0)
#else
					if(fcntl(sock, F_SETFL, flags) == 0)
#endif
						result = LNERR_OK;
				}
			}
		} else {
			if(connect(sock, addr.get_addr(), addr.get_size()) == 0)
				result = LNERR_OK;
		}
		if(result == LNERR_OK)
			skpad.Attach(addr.get_family(), sock);
	}
	return result;
}
