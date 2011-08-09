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

LNE_INLINE
ThreadSemaphore::operator bool() const
{
	return initialized_;
}

LNE_INLINE LNE_UINT
ThreadSemaphore::TryAcquire(void)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return WaitForSingleObject(sem_, 0) == WAIT_OBJECT_0 ? LNERR_OK : LNERR_TIMEOUT;
#else
	return sem_trywait(&sem_) == 0 ? LNERR_OK : LNERR_TIMEOUT;
#endif
}

LNE_INLINE LNE_UINT
ThreadSemaphore::Acquire(void)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return WaitForSingleObject(sem_, INFINITE) == WAIT_OBJECT_0 ? LNERR_OK : LNERR_UNKNOW;
#else
	return sem_wait(&sem_) == 0 ? LNERR_OK : LNERR_UNKNOW;
#endif
}

LNE_INLINE LNE_UINT
ThreadSemaphore::Release(void)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return ReleaseSemaphore(sem_, 1, NULL) ? LNERR_OK : LNERR_UNKNOW;
#else
	return sem_post(&sem_) == 0 ? LNERR_OK : LNERR_UNKNOW;
#endif
}
