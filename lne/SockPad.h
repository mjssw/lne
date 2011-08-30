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

#ifndef LNE_SOCKPAD_H
#define LNE_SOCKPAD_H

#include "config.h"

LNE_NAMESPACE_BEGIN

class SockPad
{
public:
	SockPad(void);
	~SockPad(void);
	SockPad(SockPad &other);
	SockPad &operator = (SockPad &other);

	operator bool(void);
	int family();
	SOCKET socket();
	void Attach(int family, SOCKET sock);
	void Close();

private:
	int family_;
	SOCKET socket_;
};

LNE_INLINE
SockPad::SockPad(void)
{
	family_ = AF_UNSPEC;
	socket_ = INVALID_SOCKET;
}

LNE_INLINE
SockPad::~SockPad(void)
{
	Close();
}

LNE_INLINE
SockPad::SockPad(SockPad &other)
{
	family_ = AF_UNSPEC;
	socket_ = INVALID_SOCKET;
	operator=(other);
}

LNE_INLINE SockPad &
SockPad::operator = (SockPad &other)
{
	Attach(other.family_, other.socket_);
	other.family_ = AF_UNSPEC;
	other.socket_ = INVALID_SOCKET;
	return *this;
}

LNE_INLINE
SockPad::operator bool(void)
{
	return socket_ != INVALID_SOCKET;
}

LNE_INLINE int
SockPad::family()
{
	return family_;
}

LNE_INLINE SOCKET
SockPad::socket()
{
	return socket_;
}

LNE_INLINE void
SockPad::Attach(int family, SOCKET sock)
{
	if(socket_ != INVALID_SOCKET)
		closesocket(socket_);
	family_ = family;
	socket_ = sock;
}

LNE_INLINE void
SockPad::Close()
{
	family_ = AF_UNSPEC;
	if(socket_ != INVALID_SOCKET) {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}
}

LNE_NAMESPACE_END

#endif
