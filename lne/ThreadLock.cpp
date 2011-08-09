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

#include "ThreadLock.h"

LNE_NAMESPACE_USING

#if !defined(LNE_WIN32)

int ThreadLock::pthread_mutex_spinlock(pthread_mutex_t *mutex, int spin_count)
{
	for(int i = 0; i < spin_count; ++i)
		if(pthread_mutex_trylock(mutex) == 0)
			return 0;
	return pthread_mutex_lock(mutex);
}

#endif

