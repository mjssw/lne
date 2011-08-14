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

#include "DataBlockPool.h"

LNE_NAMESPACE_USING

DataBlockPool::DataBlockPool(void)
{
	cache_head_ = NULL;
	queue_head_ = NULL;
	queue_free_ = NULL;
	capacity_ = 0;
	cache_blocks_ = 0;
}

DataBlockPool::~DataBlockPool(void)
{
	// destory DataBlock object
	BlockQueue *queue;
	while(queue_free_) {
		queue = queue_free_->next;
		queue_free_->block->~DataBlock();
		queue_free_ = queue;
	}
	while(queue_head_) {
		queue = queue_head_->next;
		queue_head_->block->~DataBlock();
		queue_head_ = queue;
	}
	// free the BlockCache
	BlockCache *cache;
	while(cache_head_) {
		cache = cache_head_->next;
		// the last cannot free
		if(cache)
			free(cache_head_);
		cache_head_ = cache;
	}
}

DataBlockPool *DataBlockPool::NewInstance(LNE_UINT capacity, LNE_UINT cache_blocks)
{
	LNE_ASSERT_RETURN(capacity > 0 && cache_blocks > 0, NULL);
	char *buffer = static_cast<char *>(malloc(sizeof(DataBlockPool) + sizeof(BlockCache) + sizeof(BlockQueue) * cache_blocks + (sizeof(DataBlock) + capacity) * cache_blocks));
	if(buffer) {
		DataBlockPool *pool = new(buffer)DataBlockPool();
		pool->capacity_ = capacity;
		pool->cache_blocks_ = cache_blocks;
		pool->AppendCache(buffer + sizeof(DataBlockPool));
		return pool;
	}
	return NULL;
}

void DataBlockPool::ObjectDestroy(void)
{
	this->~DataBlockPool();
	free(this);
}

void DataBlockPool::Free(DataBlock *block)
{
	queue_lock_.Lock();
	BlockQueue *queue = queue_free_;
	queue_free_ = queue->next;
	queue->block = block;
	queue->next = queue_head_;
	queue_head_ = queue;
	queue_lock_.Unlock();
	Release();
}

DataBlock *DataBlockPool::Alloc(void)
{
	DataBlock *result = NULL;
	queue_lock_.Lock();
	if(queue_head_ == NULL) {
		char *buffer = static_cast<char *>(malloc(sizeof(BlockCache) + sizeof(BlockQueue) * cache_blocks_ + (sizeof(DataBlock) + capacity_) * cache_blocks_));
		if(buffer)
			AppendCache(buffer);
	}
	if(queue_head_) {
		BlockQueue *queue = queue_head_;
		queue_head_ = queue->next;
		result = queue->block;
		queue->block = NULL;
		queue->next = queue_free_;
		queue_free_ = queue;
	}
	queue_lock_.Unlock();
	if(result)
		AddRef();
	return result;
}

void DataBlockPool::AppendCache(char *buffer)
{
	BlockCache *cache = reinterpret_cast<BlockCache *>(buffer);
	BlockQueue *queue = reinterpret_cast<BlockQueue *>(buffer + sizeof(BlockCache));
	buffer += sizeof(BlockCache) + sizeof(BlockQueue) * cache_blocks_;
	DataBlock *block;
	for(LNE_UINT i = 0; i < cache_blocks_; ++i) {
		block = new(buffer) DataBlock();
		block->pool_ = this;
		block->capacity_ = capacity_;
		block->buffer_ = buffer + sizeof(DataBlock);
		queue[i].block = block;
		queue[i].next = queue_head_;
		queue_head_ = &queue[i];
		buffer += sizeof(DataBlock) + capacity_;
	}
	cache->next = cache_head_;
	cache_head_ = cache;
}
