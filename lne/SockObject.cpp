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

#include "SockObject.h"

LNE_NAMESPACE_USING

SockPoolable::~SockPoolable(void)
{
}

void SockPoolable::ObjectDestroy(void)
{
	Clean();
	if(factory_) {
		SetRef(1);
		factory_->PushObject(this);
	} else
		delete this;
}

SockFactory::SockFactory(LNE_UINT limit_cache)
{
	limit_cache_ = limit_cache;
}

SockFactory::~SockFactory(void)
{
	SockPoolable *object;
	while(objects_.Pop(object) == LNERR_OK)
		delete object;
}

void SockFactory::PushObject(SockPoolable *object)
{
	lock_.Lock();
	if(objects_.get_count() > limit_cache_ || objects_.Push(object) != LNERR_OK)
		delete object;
	lock_.Unlock();
}

SockPoolable *SockFactory::PopObject(void)
{
	SockPoolable *object;
	lock_.Lock();
	if(objects_.Pop(object) != LNERR_OK)
		object = NULL;
	lock_.Unlock();
	return object;
}

void SockFactory::ObjectDestroy(void)
{
	delete this;
}
