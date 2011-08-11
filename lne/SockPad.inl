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

LNE_INLINE
SockPad::SockPad(void)
{
	socket_ = INVALID_SOCKET;
}

LNE_INLINE
SockPad::~SockPad(void)
{
	if(socket_ != INVALID_SOCKET)
		closesocket(socket_);
}

LNE_INLINE
SockPad::SockPad(SockPad &other)
{
	operator=(other);
}

LNE_INLINE SockPad &
SockPad::operator = (SockPad &other)
{
	Attach(other.Detach());
	return *this;
}

LNE_INLINE SockPad &
SockPad::operator = (SOCKET sock)
{
	Attach(sock);
	return *this;
}

LNE_INLINE
SockPad::operator bool(void)
{
	return socket_ != INVALID_SOCKET;
}

LNE_INLINE
SockPad::operator SOCKET(void)
{
	return socket_;
}

LNE_INLINE void
SockPad::Attach(SOCKET sock)
{
	if(sock != socket_ && socket_ != INVALID_SOCKET)
		closesocket(socket_);
	socket_ = sock;
}

LNE_INLINE SOCKET
SockPad::Detach(void)
{
	SOCKET sock = socket_;
	socket_ = INVALID_SOCKET;
	return sock;
}
