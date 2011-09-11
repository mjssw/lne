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

#ifndef LNE_DATABLOCK_H
#define LNE_DATABLOCK_H

#include "ExtendObject.h"

LNE_NAMESPACE_BEGIN

class DataBlockPool;

class LNE_Export DataBlock: public RefObject
{
	friend class DataBlockPool;
public:
	static const LNE_UINT DEFAULT_CAPACITY = 1500;

	static DataBlock *NewInstance(LNE_UINT capacity = DEFAULT_CAPACITY);

	bool IsFull(void) const;
	bool IsEmpty(void) const;
	char *buffer(void);
	char *buffer_free(void);
	char *buffer_end(void);
	const char *buffer(void) const;
	const char *buffer_free(void) const;
	const char *buffer_end(void) const;
	LNE_UINT size(void) const;
	LNE_UINT set_size(LNE_UINT size);
	LNE_UINT capacity(void) const;
	LNE_UINT free_size(void) const;

private:
	DataBlock(void);
	~DataBlock(void);
	void ObjectDestroy(void);

	LNE_UINT size_; // customer variable, <= capacity_
	char *buffer_;
	LNE_UINT capacity_;
	DataBlockPool *pool_;
};

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
	LNE_UINT capacity(void) const;

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
};

LNE_INLINE bool
DataBlock::IsFull(void) const
{
	return size_ == capacity_;
}

LNE_INLINE bool
DataBlock::IsEmpty(void) const
{
	return size_ == 0;
}

LNE_INLINE char *
DataBlock::buffer(void)
{
	return buffer_;
}

LNE_INLINE const char *
DataBlock::buffer(void) const
{
	return buffer_;
}

LNE_INLINE char *
DataBlock::buffer_free(void)
{
	return buffer_ + size_;
}

LNE_INLINE const char *
DataBlock::buffer_free(void) const
{
	return buffer_ + size_;
}

LNE_INLINE char *
DataBlock::buffer_end(void)
{
	return buffer_ + capacity_;
}

LNE_INLINE const char *
DataBlock::buffer_end(void) const
{
	return buffer_ + capacity_;
}

LNE_INLINE LNE_UINT
DataBlock::size(void) const
{
	return size_;
}

LNE_INLINE LNE_UINT
DataBlock::capacity(void) const
{
	return capacity_;
}

LNE_INLINE LNE_UINT
DataBlock::free_size(void) const
{
	return capacity_ - size_;
}

LNE_INLINE LNE_UINT
DataBlock::set_size(LNE_UINT size)
{
	if(size <= capacity_) {
		size_ = size;
		return LNERR_OK;
	}
	return LNERR_PARAMETER;
}

LNE_INLINE LNE_UINT
DataBlockPool::capacity(void) const
{
	return capacity_;
}

LNE_NAMESPACE_END

#endif
