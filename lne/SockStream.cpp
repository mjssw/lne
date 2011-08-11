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

#include "SockStream.h"

LNE_NAMESPACE_USING

SockStream::SockStream(void)
{
	socket_ = INVALID_SOCKET;
}

SockStream::~SockStream(void)
{
	if(socket_ != INVALID_SOCKET)
		closesocket(socket_);
}

LNE_UINT SockStream::GetSockAddr(SockAddr &addr)
{
	char address[sizeof(sockaddr_in6)];
	socklen_t len = sizeof(sockaddr_in6);
	if(getsockname(socket_, reinterpret_cast<sockaddr *>(address), &len) != 0)
		return LNERR_UNKNOW;
	return addr.Set(reinterpret_cast<sockaddr *>(address), len);
}

LNE_UINT SockStream::GetPeerAddr(SockAddr &addr)
{
	char address[sizeof(sockaddr_in6)];
	socklen_t len = sizeof(sockaddr_in6);
	if(getpeername(socket_, reinterpret_cast<sockaddr *>(address), &len) != 0)
		return LNERR_UNKNOW;
	return addr.Set(reinterpret_cast<sockaddr *>(address), len);
}
