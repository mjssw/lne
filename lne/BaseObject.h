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

#ifndef LNE_BASEOBJECT_H
#define LNE_BASEOBJECT_H

#include "config.h"

LNE_NAMESPACE_BEGIN

class LNE_Export Abstract
{
public:
	Abstract(void);
	virtual ~Abstract(void) = 0;
};

class LNE_Export Available
{
public:
	Available(void);
	operator bool(void) const;
	bool IsAvailable(void) const;
protected:
	void set_available(bool available);
private:
	bool available_;
};

LNE_INLINE
Available::operator bool(void) const
{
	return available_;
}

LNE_INLINE bool
Available::IsAvailable(void) const
{
	return available_;
}

LNE_INLINE void
Available::set_available(bool available)
{
	available_ = available;
}

LNE_NAMESPACE_END

#endif
