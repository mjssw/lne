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


LNE_INLINE SockSprayHandler *
SockSpray::get_handler(void)
{
	return handler_;
}

LNE_INLINE void *
SockSpray::get_context(void)
{
	return context_;
}

LNE_INLINE
SockSprayFactory::SockSprayFactory(LNE_UINT limit_factroy_cache)
	: SockFactory(limit_factroy_cache)
{
	pool_ = NULL;
	limit_write_cache_ = 0;
}

