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

#ifndef LNE_SOCKCONNECTOR_H
#define LNE_SOCKCONNECTOR_H

#include "BaseObject.h"
#include "SockAddr.h"
#include "TimeValue.h"
#include "SockWaves.h"

LNE_NAMESPACE_BEGIN

class LNE_Export SockConnector
{
public:
	static SockConnector *NewInstance(const SockAddr &addr, const TimeValue *tv = NULL);
	void Release(void);
	
	LNE_UINT Connect(SockPad& skpad);
	static LNE_UINT Connect(SockPad& skpad, const SockAddr &addr);
	static LNE_UINT Connect(SockPad& skpad, const SockAddr &addr, const TimeValue &tv);
	static LNE_UINT Connect(SockPad& skpad, const SockAddr &addr, const TimeValue *tv);

private:
	SockConnector(void);
	~SockConnector(void);

	SockAddr addr_;
	bool use_timeout_;
	TimeValue timeout_;
};

#include "SockConnector.inl"

LNE_NAMESPACE_END

#endif
