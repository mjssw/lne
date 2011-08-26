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

#ifndef LNE_EXTENDOBJECT_H
#define LNE_EXTENDOBJECT_H

#include "BaseObject.h"
#include "ThreadLock.h"

LNE_NAMESPACE_BEGIN

class LNE_Export RefObject: public Abstract
{
public:
	void AddRef(void);
	void Release(void);

protected:
	RefObject(void);
	void SetRef(LNE_UINT ref);
	virtual void ObjectDestroy() = 0;

private:
	ThreadLock lock_;
	LNE_UINT reference_;
};

LNE_INLINE void
RefObject::AddRef(void)
{
	lock_.Lock();
	++reference_;
	lock_.Unlock();
}

LNE_INLINE void
RefObject::SetRef(LNE_UINT ref)
{
	lock_.Lock();
	reference_ = ref;
	lock_.Unlock();
}

LNE_NAMESPACE_END

#endif
