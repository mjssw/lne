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

#ifndef LNE_DATABLOCKPOOL_H
#define LNE_DATABLOCKPOOL_H

#include "DataBlock.h"

LNE_NAMESPACE_BEGIN

class LNE_Export DataBlockPool: public RefObject
{
	friend class DataBlock;
	struct BlockQueue {
		BlockQueue *next;
		DataBlock *block;
	};
	struct BlockCache {
		BlockCache *next;
		BlockQueue queue[0];
	};
public:
	static const LNE_UINT DEFAULT_CACHE_BLOCKS = 128;

	static DataBlockPool *NewInstance(LNE_UINT capacity = DataBlock::DEFAULT_CAPACITY, LNE_UINT cache_blocks = DEFAULT_CACHE_BLOCKS);

	DataBlock *Alloc(void);
	LNE_UINT get_capacity(void) const;

private:
	DataBlockPool(void);
	~DataBlockPool(void);
	void ObjectDestroy(void);
	void Free(DataBlock *block);
	void AppendCache(char *buffer);

	LNE_UINT capacity_;
	LNE_UINT cache_blocks_;
	BlockCache *cache_head_;
	BlockQueue *queue_head_;
	BlockQueue *queue_free_;
	ThreadLock queue_lock_;
};

#include "DataBlockPool.inl"

LNE_NAMESPACE_END

#endif
