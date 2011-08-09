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
DataBlock::get_buffer(void)
{
	return buffer_;
}

LNE_INLINE const char *
DataBlock::get_buffer(void) const
{
	return buffer_;
}

LNE_INLINE char *
DataBlock::get_buffer_free(void)
{
	return buffer_ + size_;
}

LNE_INLINE const char *
DataBlock::get_buffer_free(void) const
{
	return buffer_ + size_;
}

LNE_INLINE char *
DataBlock::get_buffer_end(void)
{
	return buffer_ + capacity_;
}

LNE_INLINE const char *
DataBlock::get_buffer_end(void) const
{
	return buffer_ + capacity_;
}

LNE_INLINE LNE_UINT
DataBlock::get_size(void) const
{
	return size_;
}

LNE_INLINE LNE_UINT
DataBlock::get_capacity(void) const
{
	return capacity_;
}

LNE_INLINE LNE_UINT
DataBlock::get_free_size(void) const
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

