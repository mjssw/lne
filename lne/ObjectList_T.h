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

#ifndef LNE_OBJECTLIST_H
#define LNE_OBJECTLIST_H

#include "BaseObject.h"

LNE_NAMESPACE_BEGIN

template<typename T, LNE_UINT cache_nodes_ = 128>
class ObjectList
{
	struct ObjectNode {
		ObjectNode *prev;
		ObjectNode *next;
		T object;
	};
	struct ObjectMemory {
		ObjectMemory *next;
		ObjectNode nodes[cache_nodes_];
	};

public:
	ObjectList(void);
	~ObjectList(void);

	LNE_UINT PopFront(T &object);
	LNE_UINT PushFront(const T &object);
	LNE_UINT PopBack(T &object);
	LNE_UINT PushBack(const T &object);

	bool IsEmpty(void) const;
	LNE_UINT count(void) const;

private:
	ObjectList(const ObjectList &);
	ObjectList &operator = (const ObjectList &);
	void ExtendMemory();

	ObjectMemory *memory_head_;
	LNE_UINT nodes_count_;
	ObjectNode *nodes_circle_;
	ObjectNode *nodes_free_;
	ObjectNode init_nodes_[cache_nodes_];
};

template<typename T, LNE_UINT cache_nodes_>
ObjectList<T, cache_nodes_>::ObjectList(void)
{
	memory_head_ = NULL;
	nodes_count_ = 0;
	nodes_circle_ = NULL;
	nodes_free_ = NULL;
	for(LNE_UINT i = 0; i < cache_nodes_; ++i) {
		init_nodes_[i].next = nodes_free_;
		nodes_free_ = &init_nodes_[i];
	}
}

template<typename T, LNE_UINT cache_nodes_>
ObjectList<T, cache_nodes_>::~ObjectList(void)
{
	ObjectMemory *next;
	while(memory_head_) {
		next = memory_head_->next;
		free(memory_head_);
		memory_head_ = next;
	}
}

template<typename T, LNE_UINT cache_nodes_>
LNE_UINT ObjectList<T, cache_nodes_>::PopFront(T &object)
{
	if(nodes_circle_ == NULL)
		return LNERR_NODATA;
	ObjectNode *node = nodes_circle_;
	if(node->next == node)
		nodes_circle_ = NULL;
	else {
		nodes_circle_ = node->next;
		nodes_circle_->prev = node->prev;
		nodes_circle_->prev->next = nodes_circle_;
	}
	object = node->object;
	node->prev = NULL;
	node->next = nodes_free_;
	nodes_free_ = node;
	return LNERR_OK;
}

template<typename T, LNE_UINT cache_nodes_>
LNE_UINT ObjectList<T, cache_nodes_>::PushFront(const T &object)
{
	if(nodes_free_ == NULL)
		ExtendMemory();
	if(nodes_free_ == NULL)
		return LNERR_NOMEMORY;
	ObjectNode *node = nodes_free_;
	nodes_free_ = node->next;
	if(nodes_circle_ == NULL) {
		node->next = node;
		node->prev = node;
		nodes_circle_ = node;
	} else {
		node->next = nodes_circle_;
		node->prev = nodes_circle_->prev;
		node->prev->next = node;
		node->next->prev = node;
		nodes_circle_ = node;
	}
	node->object = object;
	return LNERR_OK;
}

template<typename T, LNE_UINT cache_nodes_>
LNE_UINT ObjectList<T, cache_nodes_>::PopBack(T &object)
{
	if(nodes_circle_ == NULL)
		return LNERR_NODATA;
	ObjectNode *node = nodes_circle_->prev;
	if(node->prev == node)
		nodes_circle_ = NULL;
	else {
		nodes_circle_->prev = node->prev;
		nodes_circle_->prev->next = nodes_circle_;
	}
	object = node->object;
	node->prev = NULL;
	node->next = nodes_free_;
	nodes_free_ = node;
	return LNERR_OK;
}

template<typename T, LNE_UINT cache_nodes_>
LNE_UINT ObjectList<T, cache_nodes_>::PushBack(const T &object)
{
	if(nodes_free_ == NULL)
		ExtendMemory();
	if(nodes_free_ == NULL)
		return LNERR_NOMEMORY;
	ObjectNode *node = nodes_free_;
	nodes_free_ = node->next;
	if(nodes_circle_ == NULL) {
		node->next = node;
		node->prev = node;
		nodes_circle_ = node;
	} else {
		node->next = nodes_circle_;
		node->prev = nodes_circle_->prev;
		node->prev->next = node;
		node->next->prev = node;
	}
	node->object = object;
	return LNERR_OK;
}

template<typename T, LNE_UINT cache_nodes_>
void ObjectList<T, cache_nodes_>::ExtendMemory()
{
	ObjectMemory *memory = static_cast<ObjectMemory *>(malloc(sizeof(ObjectMemory)));
	if(memory) {
		memory->next = memory_head_;
		memory_head_ = memory;
		for(LNE_UINT i = 0; i < cache_nodes_; ++i) {
			memory->nodes[i].prev = NULL;
			memory->nodes[i].next = nodes_free_;
			nodes_free_ = &memory->nodes[i];
		}
	}
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
bool ObjectList<T, cache_nodes_>::IsEmpty(void) const
{
	return count() == 0;
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
LNE_UINT ObjectList<T, cache_nodes_>::count(void) const
{
	return nodes_count_;
}

LNE_NAMESPACE_END

#endif
