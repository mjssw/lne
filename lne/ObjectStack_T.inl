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

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
ObjectStack<T, cache_nodes_>::ObjectStack(void)
{
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
ObjectStack<T, cache_nodes_>::~ObjectStack(void)
{
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
LNE_UINT ObjectStack<T, cache_nodes_>::Pop(T &object)
{
	return list_.PopBack(object);
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
LNE_UINT ObjectStack<T, cache_nodes_>::Push(const T &object)
{
	return list_.PushBack(object);
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
bool ObjectStack<T, cache_nodes_>::IsEmpty(void) const
{
	return get_count() == 0;
}

template<typename T, LNE_UINT cache_nodes_> LNE_INLINE
LNE_UINT ObjectStack<T, cache_nodes_>::get_count(void) const
{
	return list_.get_count();
}
