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

#include "BaseObject.h"

LNE_NAMESPACE_BEGIN

class SockPad
{
public:
	SockPad(void);
	~SockPad(void);
	SockPad(SockPad &other);
	SockPad &operator = (SockPad &other);

	operator bool(void);
	int get_family();
	SOCKET get_socket();
	void Attach(int family, SOCKET sock);
	void Close();

private:
	int family_;
	SOCKET socket_;
};

#include "SockPad.inl"

LNE_NAMESPACE_END

#endif
