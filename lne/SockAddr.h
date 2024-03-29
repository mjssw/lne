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

#include "config.h"

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
	LNE_UINT Set(LNE_UINT16 port, int address_family = AF_UNSPEC);
	LNE_UINT Set(const char *address, int address_family = AF_UNSPEC);
	LNE_UINT Set(LNE_UINT16 port, const char *address, int address_family = AF_UNSPEC);

	LNE_UINT family(void) const;
	LNE_UINT size(void) const;
	const sockaddr *addr(void) const;
	const char *addr_text(void) const;

	// WARNING: only used for LNE
	sockaddr *ready_raw_addr(void);
	socklen_t &ready_raw_size(void);
	void generate_addr_text(void);

private:
	void Reset();
	LNE_UINT Set(const sockaddr *addr, socklen_t len);

	socklen_t size_;
	union {
		sockaddr     sa;
		sockaddr_in  in4;
		sockaddr_in6 in6;
	} addr_;
	char addr_text_[INET6_ADDRSTRLEN + 1];
};

LNE_INLINE void
SockAddr::Set(const sockaddr_in &in4)
{
	Set(reinterpret_cast<const sockaddr *>(&in4), sizeof(in4));
}

LNE_INLINE void
SockAddr::Set(const sockaddr_in6 &in6)
{
	Set(reinterpret_cast<const sockaddr *>(&in6), sizeof(in6));
}

LNE_INLINE
SockAddr::operator bool()
{
	return addr_.sa.sa_family != AF_UNSPEC && size_ > 0;
}

LNE_INLINE LNE_UINT
SockAddr::family(void) const
{
	return addr_.sa.sa_family;
}

LNE_INLINE LNE_UINT
SockAddr::size(void) const
{
	return size_;
}

LNE_INLINE const sockaddr *
SockAddr::addr(void) const
{
	return &addr_.sa;
}

LNE_INLINE const char *
SockAddr::addr_text(void) const
{
	return addr_text_;
}

LNE_INLINE sockaddr *
SockAddr::ready_raw_addr(void)
{
	return &addr_.sa;
}

LNE_INLINE socklen_t &
SockAddr::ready_raw_size(void)
{
	size_ = sizeof(addr_);
	return size_;
}

LNE_NAMESPACE_END

#endif
