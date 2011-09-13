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

#include "SockEventer.h"

LNE_NAMESPACE_USING

SockEventer::SockEventer(SockEventerPool *pool)
{
	pool_ = pool;
	poller_ = NULL;
	prev_ = next_ = NULL;
}

void SockEventer::ObjectDestroy(void)
{
	Clean();
	if(pool_) {
		SetRef(1);
		set_poller(NULL);
		set_prev(NULL);
		set_next(NULL);
		set_context(NULL);
		pool_->PushObject(this);
	} else
		delete this;
}

void SockEventer::HandleRead(void)
{
}

void SockEventer::HandleWrite(void)
{
}

void SockEventer::HandleShutdown(void)
{
}

void SockEventer::HandleIdleTimeout(void)
{
}

SockEventerPool::SockEventerPool(LNE_UINT limit_cache)
{
	limit_cache_ = limit_cache;
}

SockEventerPool::~SockEventerPool(void)
{
	SockEventer *object;
	while(objects_.Pop(object) == LNERR_OK)
		delete object;
}

void SockEventerPool::PushObject(SockEventer *object)
{
	RefLock();
	if(objects_.count() > limit_cache_ || objects_.Push(object) != LNERR_OK)
		delete object;
	RefUnlock();
	Release();
}

SockEventer *SockEventerPool::PopObject(void)
{
	SockEventer *object;
	RefLock();
	if(objects_.Pop(object) != LNERR_OK)
		object = NULL;
	RefUnlock();
	if(object)
		AddRef();
	return object;
}

void SockEventerPool::ObjectDestroy(void)
{
	delete this;
}
