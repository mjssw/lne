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

LNE_UINT SockStream::GetSockAddr(SockAddr &addr)
{
	if(getsockname(skpad_.socket(), addr.ready_raw_addr(), &addr.ready_raw_size()) == 0) {
		addr.generate_addr_text();
		return LNERR_OK;
	}
	return LNERR_UNKNOW;
}

LNE_UINT SockStream::GetPeerAddr(SockAddr &addr)
{
	if(getpeername(skpad_.socket(), addr.ready_raw_addr(), &addr.ready_raw_size()) == 0) {
		addr.generate_addr_text();
		return LNERR_OK;
	}
	return LNERR_UNKNOW;
}
