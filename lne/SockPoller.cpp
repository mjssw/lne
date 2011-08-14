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

#include "SockPoller.h"

LNE_NAMESPACE_USING

SockPoller::SockPoller(LNE_UINT workers)
	: lock_(true)
{
	threads_ = NULL;
	thread_workers_ = 0;
	exit_request_ = false;
	set_available(true);
	//create poller
#if defined(LNE_WIN32)
	poller_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
#elif defined(LNE_LINUX)
	poller_ = epoll_create(1);
#elif defined(LNE_FREEBSD)
	poller_ = kqueue();
#endif
	if(poller_ == INVALID_POLLER)
		set_available(false);
	//create thread pool
	if(IsAvailable()) {
#if defined(LNE_WIN32)
		threads_ = static_cast<HANDLE *>(malloc(sizeof(HANDLE) * workers));
#else
		threads_ = static_cast<pthread_t *>(malloc(sizeof(pthread_t) * workers));
#endif
		if(threads_ == NULL)
			set_available(false);
	}
	if(IsAvailable()) {
		for(LNE_UINT i = 0; i < workers; ++i) {
#if defined(LNE_WIN32)
			threads_[i] = CreateThread(NULL, 0, ThreadRoutine, this, 0, NULL);
			if(threads_[i] == NULL)
#else
			if(pthread_create(&threads_[i], NULL, ThreadRoutine, this) != 0)
#endif
				break;
			++thread_workers_;
		}
		if(thread_workers_ < workers)
			set_available(false);
	}
}

SockPoller::~SockPoller(void)
{
	exit_request_ = true;
	for(LNE_UINT i = 0; i < thread_workers_; ++i) {
#if defined(LNE_WIN32)
		WaitForSingleObject(threads_[i], INFINITE);
		CloseHandle(threads_[i]);
#else
		pthread_join(threads_[i], NULL);
#endif
	}
	if(poller_ != INVALID_POLLER)
		closepoller(poller_);
	if(threads_)
		free(threads_);
}

SockPoller *SockPoller::NewInstance(LNE_UINT workers)
{
	LNE_ASSERT_RETURN(workers > 0, NULL);
	SockPoller *retval = NULL;
	try {
		retval = new SockPoller(workers);
		if(retval) {
			if(!retval->IsAvailable()) {
				delete retval;
				retval = NULL;
			}
		}
	} catch(std::bad_alloc) {
	}
	return retval;
}

void SockPoller::ObjectDestroy(void)
{
	delete this;
}

LNE_UINT SockPoller::Managed(SockEventer *eventer)
{
	LNE_ASSERT_RETURN(eventer != NULL, LNERR_PARAMETER);
	return eventer->Bind(poller_) ? LNERR_OK : LNERR_UNKNOW;
}

#if defined(LNE_WIN32)
DWORD WINAPI SockPoller::ThreadRoutine(LPVOID parameter)
{
	((SockPoller *)parameter)->Service();
	return 0;
}
#else
void *SockPoller::ThreadRoutine(void *parameter)
{
	((SockPoller *)parameter)->Service();
	return (void *)0;
}
#endif

#if defined(LNE_WIN32)
void SockPoller::Service(void)
{
	DWORD bytes;
	ULONG_PTR key;
	SockEventer::IOCP_OVERLAPPED *overlap;
	do {
		if(GetQueuedCompletionStatus(poller_, &bytes, &key, reinterpret_cast<LPOVERLAPPED *>(&overlap), 500)) {
			if(overlap->type == SockEventer::IOCP_READ)
				overlap->owner->HandleRead();
			else if(overlap->type == SockEventer::IOCP_WRITE)
				overlap->owner->HandleWrite();
			else
				overlap->owner->HandleShutdown();
		}
	} while(!exit_request_);
}
#elif defined(LNE_LINUX)
void SockPoller::Service(void)
{
	int rc;
	SockEventer *client;
	struct epoll_event event;
	do {
		rc = epoll_wait(poller_, &event, 1, 500);
		if(rc > 0) {
			client = reinterpret_cast<SockEventer *>(event.data.ptr);
			if(event.events & EPOLLIN)
				client->HandleRead();
			if(event.events & EPOLLOUT)
				client->HandleWrite();
			if(event.events & (EPOLLERR | EPOLLHUP))
				client->HandleShutdown();
		}
	} while(!exit_request_);
}
#elif defined(LNE_FREEBSD)
void SockPoller::Service(void)
{
	int rc;
	SockEventer *client;
	struct timespec timeout;
	struct kevent event, kev;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 500 * 1000000;
	do {
		rc = kevent(poller_, NULL, 0, &event, 1, &timeout);
		if(rc > 0) {
			client = reinterpret_cast<SockEventer *>(event.udata);
			if(event.flags & (EV_EOF | EV_ERROR)) {
				EV_SET(&kev, event.ident, event.filter, EV_DELETE, 0, 0, NULL);
				kevent(poller_, &kev, 1, NULL, 0, NULL);
				client->HandleShutdown();
			} else {
				if(event.filter == EVFILT_READ)
					client->HandleRead();
				else if(event.filter == EVFILT_WRITE)
					client->HandleWrite();
			}
		}
	} while(!exit_request_);
}
#endif

