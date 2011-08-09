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
}

SockStream::~SockStream(void)
{
}

SockStream *SockStream::NewInstance(SOCKET sock)
{
	LNE_ASSERT(sock != INVALID_SOCKET, NULL);
	SockStream *result = NULL;
	try {
		result = new SockStream();
		result->socket_ = sock;
	} catch(std::bad_alloc) {
	}
	return result;
}

void SockStream::Release()
{
	delete this;
}

LNE_UINT SockStream::Send(DataBlock *block)
{
	LNE_ASSERT(block != NULL && block->get_size() > 0, LNERR_PARAMETER);
	ssize_t len;
#if defined(LNE_WIN32)
	len = send(socket_, block->get_buffer(), block->get_size(), 0);
#else
	do {
		len = send(socket_, block->get_buffer(), block->get_size(), MSG_NOSIGNAL);
	} while(len == SOCKET_ERROR && errno == EINTR);
#endif
	if(len < 0)
		return LNERR_UNKNOW;
	return LNERR_OK;
}

LNE_UINT SockStream::Recv(DataBlock *block, const TimeValue *tv)
{
	LNE_ASSERT(block != NULL && block->get_capacity() > 0, LNERR_PARAMETER);
	if(tv) {
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(socket_, &fds);
		TimeValue timeout(*tv);
		if(select(socket_ + 1, &fds, NULL, NULL, static_cast<timeval *>(timeout)) < 1)
			return LNERR_TIMEOUT;
	}
	ssize_t len;
#if defined(LNE_WIN32)
	len = recv(socket_, block->get_buffer(), block->get_capacity(), 0);
#else
	do {
		len = recv(socket_, block->get_buffer(), block->get_capacity(), 0);
	} while(len == SOCKET_ERROR && errno == EINTR);
#endif
	if(len < 0)
		return LNERR_UNKNOW;
	else if(len == 0)
		return LNERR_IOCLOSED;
	block->set_size(len);
	return LNERR_OK;
}
