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

#include "ThreadPool.h"
#include <algorithm>

LNE_NAMESPACE_USING

ThreadTask::ThreadTask(void)
	: next_(NULL)
{
}

ThreadTask::~ThreadTask(void)
{
}

ThreadPool::ThreadPool(LNE_UINT init, LNE_UINT limit)
{
	init = std::min(init, limit);
	threads_ = NULL;
	num_thread_ = 0;
	run_thread_ = 0;
	limit_thread_ = limit;
	initialized_ = true;
	exit_request_ = false;
	num_tasks_ = 0;
	tasks_head_ = NULL;
	tasks_rear_ = NULL;
	if(!condition_)
		initialized_ = false;
	if(initialized_) {
#if defined(LNE_WIN32)
		threads_ = static_cast<HANDLE *>(malloc(sizeof(HANDLE) * limit));
#else
		threads_ = static_cast<pthread_t *>(malloc(sizeof(pthread_t) * limit));
#endif
		if(threads_ == NULL)
			initialized_ = false;
	}
	if(initialized_) {
		for(LNE_UINT i = 0; i < init; ++i) {
#if defined(LNE_WIN32)
			threads_[i] = CreateThread(NULL, 0, ThreadRoutine, this, 0, NULL);
			if(threads_[i] == NULL)
#else
			if(pthread_create(&threads_[i], NULL, ThreadRoutine, this) != 0)
#endif
				break;
			++num_thread_;
		}
		if(num_thread_ < init)
			initialized_ = false;
	}
}

ThreadPool::~ThreadPool(void)
{
	exit_request_ = true;
	condition_.Signal();
	for(LNE_UINT i = 0; i < num_thread_; ++i) {
#if defined(LNE_WIN32)
		WaitForSingleObject(threads_[i], INFINITE);
		CloseHandle(threads_[i]);
#else
		pthread_join(threads_[i], NULL);
#endif
	}
	while(tasks_head_) {
		tasks_rear_ = tasks_head_->get_next();
		tasks_head_->Discard();
		tasks_head_ = tasks_rear_;
	}
	if(threads_)
		free(threads_);
}

ThreadPool *ThreadPool::NewInstance(LNE_UINT limit)
{
	LNE_ASSERT(limit > 0, NULL);
	LNE_UINT init;
	ThreadPool *retval = NULL;
	if(limit > 0) {
		init = (limit / 5 < 1) ? 1 : (limit / 5);
		try {
			retval = new ThreadPool(init, limit);
			if(retval) {
				if(!retval->initialized_) {
					delete retval;
					retval = NULL;
				}
			}
		} catch(std::bad_alloc) {
		}
	}
	return retval;
}

void ThreadPool::Release(void)
{
	delete this;
}

LNE_UINT ThreadPool::PostTask(ThreadTask *task)
{
	LNE_UINT retval = LNERR_UNKNOW;
	lock_.Lock();
	++num_tasks_;
	if(tasks_rear_ == NULL)
		tasks_head_ = tasks_rear_ = task;
	else {
		tasks_rear_->set_next(task);
		tasks_rear_ = task;
	}
	// auto create worker thread
	if(num_thread_ < limit_thread_) {
		if(num_tasks_ > num_thread_ - run_thread_) {
#if defined(LNE_WIN32)
			threads_[num_thread_] = CreateThread(NULL, 0, ThreadRoutine, this, 0, NULL);
			if(threads_[num_thread_])
#else
			if(pthread_create(&threads_[num_thread_], NULL, ThreadRoutine, this) == 0)
#endif
				++num_thread_;
		}
	}
	condition_.Signal();
	lock_.Unlock();
	return retval;
}

#if defined(LNE_WIN32)
DWORD WINAPI ThreadPool::ThreadRoutine(LPVOID parameter)
{
	((ThreadPool *)parameter)->Service();
	return 0;
}
#else
void *ThreadPool::ThreadRoutine(void *parameter)
{
	((ThreadPool *)parameter)->Service();
	return (void *)0;
}
#endif

void ThreadPool::Service(void)
{
	ThreadTask *task;
	do {
		if(condition_.Wait() != LNERR_OK || exit_request_)
			break;
		do {
			task = NULL;
			lock_.Lock();
			if(tasks_head_) {
				--num_tasks_;
				task = tasks_head_;
				tasks_head_ = task->get_next();
				if(tasks_head_ == NULL)
					tasks_rear_ = NULL;
				task->set_next(NULL);
				++run_thread_;
			}
			lock_.Unlock();
			if(task) {
				task->Service();
				lock_.Lock();
				--run_thread_;
				lock_.Unlock();
			}
		} while(!exit_request_ && task != NULL);
	} while(!exit_request_);
	// transfer exit action
	condition_.Signal();
	// cleanup untreated task
	do {
		task = NULL;
		lock_.Lock();
		if(tasks_head_) {
			--num_tasks_;
			task = tasks_head_;
			tasks_head_ = task->get_next();
			if(tasks_head_ == NULL)
				tasks_rear_ = NULL;
			task->set_next(NULL);
		}
		lock_.Unlock();
		if(task)
			task->Discard();
	} while(task != NULL);
}
