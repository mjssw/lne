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

#include "SockAcceptor.h"

LNE_NAMESPACE_USING

SockAcceptor::SockAcceptor(void)
{
	socket_ = INVALID_SOCKET;
	use_timeout_ = false;
}

SockAcceptor::~SockAcceptor(void)
{
	if(socket_ != INVALID_SOCKET)
		closesocket(socket_);
}

SockAcceptor *SockAcceptor::NewInstance(const SockAddr &addr, LNE_UINT backlog, const TimeValue *tv)
{
	SOCKET sock = socket(addr.get_family(), SOCK_STREAM, IPPROTO_TCP);
	if(sock != INVALID_SOCKET) {
		int flag = 1;
		if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&flag), sizeof(int)) == 0
				&& bind(sock, addr.get_addr(), addr.get_size()) == 0
				&& listen(sock, backlog) == 0) {
			try {
				SockAcceptor *acceptor = new SockAcceptor();
				acceptor->socket_ = sock;
				if(tv) {
					acceptor->use_timeout_ = true;
					acceptor->timeout_ = *tv;
				}
				return acceptor;
			} catch(std::bad_alloc) {
			}
		}
		closesocket(sock);
	}
	return NULL;
}

void SockAcceptor::Release(void)
{
	delete this;
}

LNE_UINT SockAcceptor::Accept(SockPad &sock, const TimeValue *tv)
{
	LNE_ASSERT_RETURN(socket_ != INVALID_SOCKET, LNERR_PARAMETER);
	if(tv) {
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(socket_, &fds);
		TimeValue timeout(*tv);
		if(select(static_cast<int>(socket_ + 1), &fds, NULL, NULL, (timeval *)timeout) < 1)
			return LNERR_TIMEOUT;
	}
	sock = accept(socket_, NULL, NULL);
	return sock ? LNERR_OK : LNERR_UNKNOW;
}
