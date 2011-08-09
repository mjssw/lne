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

LNE_INLINE void
SockAddr::Set(const sockaddr_in &in4)
{
	Set(reinterpret_cast<const sockaddr *>(&in4), sizeof(in4));
}

LNE_INLINE void
SockAddr::Set(const sockaddr_in6 &in6)
{
	Set(reinterpret_cast<const sockaddr *>(&in6), sizeof(in6));
}

LNE_INLINE
SockAddr::operator bool()
{
	return family_ != AF_UNSPEC && size_ > 0;
}

LNE_INLINE LNE_UINT
SockAddr::get_family(void) const
{
	return family_;
}

LNE_INLINE LNE_UINT
SockAddr::get_size(void) const
{
	return size_;
}
LNE_INLINE const sockaddr *
SockAddr::get_addr(void) const
{
	return (sockaddr *)&addr_;
}

LNE_INLINE const char *
SockAddr::get_addr_text(void) const
{
	return addr_text_;
}
