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

LNE_INLINE SockEventer *
SockEventer::get_prev(void)
{
	return prev_;
}

LNE_INLINE void
SockEventer::set_prev(SockEventer *prev)
{
	prev_ = prev;
}

LNE_INLINE SockEventer *
SockEventer::get_next(void)
{
	return next_;
}

LNE_INLINE void
SockEventer::set_next(SockEventer *next)
{
	next_ = next;
}

LNE_INLINE time_t
SockEventer::get_active(void)
{
	return active_;
}

LNE_INLINE void
SockEventer::set_active(time_t active)
{
	active_ = active;
}

LNE_INLINE SockPoller *
SockEventer::get_poller(void)
{
	return poller_;
}

LNE_INLINE void
SockEventer::set_poller(SockPoller *poller)
{
	poller_ = poller;
}
