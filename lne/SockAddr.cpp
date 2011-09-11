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

#include "SockAddr.h"

LNE_NAMESPACE_USING

SockAddr::SockAddr()
{
	Reset();
}

SockAddr::SockAddr(const sockaddr_in &in4)
{
	Reset();
	Set(in4);
}

SockAddr::SockAddr(const sockaddr_in6 &in6)
{
	Reset();
	Set(in6);
}

SockAddr::SockAddr(const sockaddr *addr, int len)
{
	Reset();
	Set(addr, len);
}

SockAddr::SockAddr(LNE_UINT16 port, int address_family)
{
	Reset();
	Set(port, address_family);
}

SockAddr::SockAddr(const char *address, int address_family)
{
	Reset();
	Set(address, address_family);
}

SockAddr::SockAddr(LNE_UINT16 port, const char *address, int address_family)
{
	Reset();
	Set(port, address, address_family);
}

SockAddr::~SockAddr(void)
{
}

SockAddr::SockAddr(const SockAddr &other)
{
	operator =(other);
}

SockAddr &SockAddr::operator =(const SockAddr &other)
{
	size_ = other.size_;
	memcpy(&addr_, &other.addr_, sizeof(addr_));
	memcpy(addr_text_, other.addr_text_, sizeof(addr_text_));
	return *this;
}

LNE_UINT SockAddr::Set(LNE_UINT16 port, int address_family)
{
	if(address_family != AF_INET6) {
		memset(&addr_.in4, 0, sizeof(addr_.in4));
		addr_.in4.sin_family = AF_INET;
		addr_.in4.sin_port = htons(port);
		size_ = sizeof(addr_.in4);
		return LNERR_OK;
	}
	if(address_family != AF_INET) {
		memset(&addr_.in6, 0, sizeof(addr_.in6));
		addr_.in6.sin6_family = AF_INET6;
		addr_.in6.sin6_port = htons(port);
		size_ = sizeof(addr_.in6);
		return LNERR_OK;
	}
	return LNERR_PARAMETER;
}

LNE_UINT SockAddr::Set(const char *address, int address_family)
{
	if(address == NULL)
		return LNERR_PARAMETER;
	char *hostname = strdup(address);
	if(hostname == NULL)
		return LNERR_NOMEMORY;
	char *p = strrchr(hostname, ':');
	if(p == NULL) {
		free(hostname);
		return LNERR_PARAMETER;
	}
	*p++ = '\0';
	LNE_UINT ret = Set(atoi(p), hostname, address_family);
	free(hostname);
	return ret;
}

LNE_UINT SockAddr::Set(LNE_UINT16 port, const char *address, int address_family)
{
	if(port == 0 || address == NULL
			|| (address_family != AF_UNSPEC && address_family != AF_INET && address_family != AF_INET6))
		return LNERR_PARAMETER;
	Reset();
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0, sizeof(hints));
	if(address_family != AF_INET6) {
		sockaddr_in in4;
		// analize ip4 address
#if defined(LNE_WIN32)
		int addr_len = sizeof(in4);
		if(WSAStringToAddressA((LPSTR)address, AF_INET, NULL, (LPSOCKADDR)&in4, &addr_len) == 0) {
#else
		memset(&in4, 0, sizeof(in4));
		in4.sin_family = AF_INET;
		if(inet_pton(AF_INET, address, &in4.sin_addr) == 1) {
#endif
			in4.sin_port = htons(port);
			Set(reinterpret_cast<const sockaddr *>(&in4), sizeof(in4));
			return LNERR_OK;
		}
		// analize ip4 hostname
		hints.ai_family = AF_INET;
		if(getaddrinfo(address, NULL, &hints, &res) == 0) {
			memcpy(&in4, res->ai_addr, res->ai_addrlen);
			freeaddrinfo(res);
			in4.sin_port = htons(port);
			Set(reinterpret_cast<const sockaddr *>(&in4), sizeof(in4));
			return LNERR_OK;
		}
	}
	if(address_family != AF_INET) {
		sockaddr_in6 in6;
		// analize ip6 address
#if defined(LNE_WIN32)
		int addr_len = sizeof(in6);
		if(WSAStringToAddressA((LPSTR)address, AF_INET6, NULL, (LPSOCKADDR)&in6, &addr_len) == 0) {
#else
		memset(&in6, 0, sizeof(in6));
		in6.sin6_family = AF_INET6;
		if(inet_pton(AF_INET6, address, &in6.sin6_addr) == 1) {
#endif
			in6.sin6_port = htons(port);
			Set(reinterpret_cast<const sockaddr *>(&in6), sizeof(in6));
			return LNERR_OK;
		}
		hints.ai_family = AF_INET6;
		if(getaddrinfo(address, 0, &hints, &res) == 0) {
			memcpy(&in6, res->ai_addr, res->ai_addrlen);
			freeaddrinfo(res);
			in6.sin6_port = htons(port);
			Set(reinterpret_cast<const sockaddr *>(&in6), sizeof(in6));
			return LNERR_OK;
		}
	}
	if(res)
		freeaddrinfo(res);
	return LNERR_UNKNOW;
}

void SockAddr::Reset()
{
	size_ = 0;
	memset(&addr_, 0, sizeof(addr_));
	memset(addr_text_, 0, sizeof(addr_text_));
}

LNE_UINT SockAddr::Set(const sockaddr *addr, socklen_t len)
{
	if(addr == NULL || len <= 0)
		return LNERR_PARAMETER;
	if(addr->sa_family == AF_INET) {
		if(len < (int)sizeof(sockaddr_in))
			return LNERR_PARAMETER;
		size_ = len;
		memcpy(&addr_.in4, addr, len);
	} else if(addr->sa_family == AF_INET6) {
		if(len < (socklen_t)sizeof(sockaddr_in6))
			return LNERR_PARAMETER;
		size_ = len;
		memcpy(&addr_.in6, addr, len);;
	} else
		return LNERR_NOSUPPORT;
	generate_addr_text();
	return LNERR_OK;
}

void SockAddr::generate_addr_text(void)
{
	if(addr_.sa.sa_family == AF_INET) {
#if defined(LNE_WIN32)
		DWORD out_len = sizeof(addr_text_);
		WSAAddressToStringA((LPSOCKADDR)&addr_.in4, sizeof(addr_.in4), NULL, addr_text_, &out_len);
#else
		if(inet_ntop(AF_INET, &addr_.in4.sin_addr, addr_text_, sizeof(addr_text_)))
			sprintf(addr_text_ + strlen(addr_text_), ":%u", ntohs(addr_.in4.sin_port));
#endif
	} else if(addr_.sa.sa_family == AF_INET6) {
#if defined(LNE_WIN32)
		DWORD out_len = sizeof(addr_text_);
		WSAAddressToStringA((LPSOCKADDR)&addr_.in6, sizeof(addr_.in6), NULL, addr_text_, &out_len);
#else
		if(inet_ntop(AF_INET6, &addr_.in6.sin6_addr, addr_text_, sizeof(addr_text_)))
			sprintf(addr_text_ + strlen(addr_text_), ":%u", ntohs(addr_.in6.sin6_port));
#endif
	}
}
