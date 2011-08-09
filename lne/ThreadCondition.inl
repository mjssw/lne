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
ThreadCondition::operator bool(void) const
{
	return initialized_;
}

LNE_INLINE LNE_UINT
ThreadCondition::Wait(void)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return WaitForSingleObject(event_, INFINITE) == WAIT_OBJECT_0 ? LNERR_OK : LNERR_UNKNOW;
#else
	int ret = LNERR_UNKNOW;
	if(pthread_mutex_lock(&mutex_) == 0) {
		if(signal_ || pthread_cond_wait(&cond_, &mutex_) == 0) {
			signal_ = false;
			ret = LNERR_OK;
		}
		pthread_mutex_unlock(&mutex_);
	}
	return ret;
#endif
}

LNE_INLINE LNE_UINT
ThreadCondition::Signal(void)
{
	if(!initialized_)
		return LNERR_NOINIT;
#if defined(LNE_WIN32)
	return SetEvent(event_) ? LNERR_OK : LNERR_UNKNOW;
#else
	int ret = LNERR_UNKNOW;
	if(pthread_mutex_lock(&mutex_) == 0) {
		if(signal_)
			ret = LNERR_OK;
		else if(pthread_cond_signal(&cond_) == 0) {
			signal_ = true;
			ret = LNERR_OK;
		}
		pthread_mutex_unlock(&mutex_);
	}
	return ret;
#endif
}
