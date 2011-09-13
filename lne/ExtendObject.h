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
	RefObject(bool fast_mode = true);
	void RefLock(void);
	void RefUnlock(void);
	void SetRef(LNE_UINT ref);
	virtual void ObjectDestroy(void) = 0;

private:
	ThreadLock lock_;
	LNE_UINT reference_;
};

template<typename T>
class AutoRelease
{
public:
	AutoRelease(T *t);
	~AutoRelease(void);
	operator bool(void);
	T *operator ->(void);
	operator T *(void);

private:
	T *t_;
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

LNE_INLINE void
RefObject::RefLock(void)
{
	lock_.Lock();
}

LNE_INLINE void
RefObject::RefUnlock(void)
{
	lock_.Unlock();
}

template<typename T>
AutoRelease<T>::AutoRelease(T *t)
{
	t_ = t;
}

template<typename T>
AutoRelease<T>::~AutoRelease(void)
{
	if(t_)
		t_->Release();
}

template<typename T>
AutoRelease<T>::operator bool(void)
{
	return t_ != NULL;
}

template<typename T>
T *AutoRelease<T>::operator ->(void)
{
	return t_;
}

template<typename T>
AutoRelease<T>::operator T *(void)
{
	return t_;
}

LNE_NAMESPACE_END

#endif
