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

#ifndef LNE_SOCKSTREAM_H
#define LNE_SOCKSTREAM_H

#include "SockBase.h"
#include "TimeValue.h"
#include "DataBlock.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockStream : public SockBase
{
	friend class SockPoller;
	friend class SockAcceptor;
	friend class SockConnector;
public:
	void Release();
	LNE_UINT Send(DataBlock *block);
	LNE_UINT Recv(DataBlock *block);
	LNE_UINT Recv(DataBlock *block, const TimeValue &tv);

private:
	SockStream(void);
	~SockStream(void);
	static SockStream *NewInstance(SOCKET sock);
	LNE_UINT Recv(DataBlock *block, const TimeValue *tv);

	ThreadLock lock_;
};

#include "SockStream.inl"

LNE_NAMESPACE_END

#endif
