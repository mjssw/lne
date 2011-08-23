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

LNE_INLINE LNE_UINT
SockConnector::Connect(SockPad &skpad)
{
	return Connect(skpad, addr_, use_timeout_ ? &timeout_ : NULL);
}

LNE_INLINE LNE_UINT
SockConnector::Connect(SockPad &skpad, const SockAddr &addr)
{
	return Connect(skpad, addr, NULL);
}

LNE_INLINE LNE_UINT
SockConnector::Connect(SockPad &skpad, const SockAddr &addr, const TimeValue &tv)
{
	return Connect(skpad, addr, &tv);
}
