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

#ifndef LNE_SOCKWAVES_H
#define LNE_SOCKWAVES_H

#include "SockPad.h"
#include "SockStream.h"
#include "TimeValue.h"
#include "DataBlock.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockWaves : public SockStream
{
	friend class SockReactor;
	friend class SockAcceptor;
	friend class SockConnector;
public:
	static SockWaves *NewInstance(SockPad skpad);
	void Release();
	LNE_UINT Send(DataBlock *block);
	LNE_UINT Recv(DataBlock *block);
	LNE_UINT Recv(DataBlock *block, const TimeValue &tv);

private:
	SockWaves(void);
	~SockWaves(void);
	LNE_UINT Recv(DataBlock *block, const TimeValue *tv);
};

LNE_INLINE LNE_UINT
SockWaves::Recv(DataBlock *block)
{
	return Recv(block, NULL);
}

LNE_INLINE LNE_UINT
SockWaves::Recv(DataBlock *block, const TimeValue &tv)
{
	return Recv(block, &tv);
}

LNE_NAMESPACE_END

#endif
