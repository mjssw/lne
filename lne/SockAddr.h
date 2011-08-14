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

#ifndef LNE_SOCKADDR_H
#define LNE_SOCKADDR_H

#include "BaseObject.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockAddr
{
public:
	SockAddr();
	explicit SockAddr(const sockaddr_in &in4);
	explicit SockAddr(const sockaddr_in6 &in6);
	explicit SockAddr(const sockaddr *addr, int len);
	explicit SockAddr(LNE_UINT16 port, int address_family = AF_UNSPEC);
	explicit SockAddr(const char *address, int address_family = AF_UNSPEC);
	explicit SockAddr(LNE_UINT16 port, const char *address, int address_family = AF_UNSPEC);
	~SockAddr(void);

	operator bool();
	SockAddr(const SockAddr &other);
	SockAddr &operator =(const SockAddr &other);
	void Set(const sockaddr_in &in4);
	void Set(const sockaddr_in6 &in6);
	LNE_UINT Set(const sockaddr *addr, int len);
	LNE_UINT Set(LNE_UINT16 port, int address_family = AF_UNSPEC);
	LNE_UINT Set(const char *address, int address_family = AF_UNSPEC);
	LNE_UINT Set(LNE_UINT16 port, const char *address, int address_family = AF_UNSPEC);
	
	LNE_UINT get_family(void) const;
	LNE_UINT get_size(void) const;
	const sockaddr *get_addr(void) const;
	const char* get_addr_text(void) const;

private:
	void Reset();

	LNE_UINT family_;
	LNE_UINT size_;
	union {
		sockaddr_in  in4;
		sockaddr_in6 in6;
	} addr_;
	char addr_text_[INET6_ADDRSTRLEN];
};

#include "SockAddr.inl"

LNE_NAMESPACE_END

#endif
