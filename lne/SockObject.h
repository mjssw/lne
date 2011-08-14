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

#ifndef LNE_SOCKOBJECT_H
#define LNE_SOCKOBJECT_H

#include "ExtendObject.h"
#include "ObjectStack_T.h"

LNE_NAMESPACE_BEGIN

class SockFactory;

class LNE_Export SockPoolable : public RefObject
{
public:
	SockPoolable(SockFactory *factory);

protected:
	void ObjectDestroy(void);
	virtual void Clean(void) = 0;

private:
	SockFactory *factory_;
};

class LNE_Export SockFactory: public RefObject
{
	friend class SockPoolable;
public:
	static const LNE_UINT DEFAULT_LIMIT_CACHE = 128;

	SockFactory(LNE_UINT limit_cache);
	~SockFactory(void);

protected:
	void ObjectDestroy(void);
	void PushObject(SockPoolable* object);
	SockPoolable* PopObject(void);

private:
	ThreadLock lock_;
	LNE_UINT limit_cache_;
	ObjectStack<SockPoolable *>  objects_;
};

#include "SockObject.inl"

LNE_NAMESPACE_END

#endif
