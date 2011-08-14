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

#ifndef LNE_OBJECTQUEUE_H
#define LNE_OBJECTQUEUE_H

#include "BaseObject.h"
#include "ObjectList_T.h"

LNE_NAMESPACE_BEGIN

template<typename T, LNE_UINT cache_nodes_ = 128>
class ObjectQueue
{
public:
	ObjectQueue(void);
	~ObjectQueue(void);

	LNE_UINT Extract(T &object);
	LNE_UINT Append(const T &object);

	bool IsEmpty(void) const;
	LNE_UINT get_count(void) const;

private:
	ObjectList<T, cache_nodes_> list_;
};

#include "ObjectQueue_T.inl"

LNE_NAMESPACE_END

#endif
