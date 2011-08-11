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

SockPoller::SockPoller(DataBlockPool *pool, LNE_UINT workers, LNE_UINT limit_cache)
	: lock_(true), reference_count_(1)
{
	pool_ = pool;
	thread_workers_ = 0;
	limit_cache_ = limit_cache;
	threads_ = NULL;
	initialized_ = true;
	exit_request_ = false;
	//create poller
#if defined(LNE_WIN32)
	poller_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(poller_ == NULL)
#elif defined(LNE_LINUX)
	poller_ = epoll_create(1);
	if(poller_ < 0)
#elif defined(LNE_FREEBSD)
	poller_ = kqueue();
	if(poller_ < 0)
#endif
		initialized_ = false;
	//create thread pool
	if(initialized_) {
#if defined(LNE_WIN32)
		threads_ = static_cast<HANDLE *>(malloc(sizeof(HANDLE) * workers));
#else
		threads_ = static_cast<pthread_t *>(malloc(sizeof(pthread_t) * workers));
#endif
		if(threads_ == NULL)
			initialized_ = false;
	}
	if(initialized_) {
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
			initialized_ = false;
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
#if defined(LNE_WIN32)
	if(poller_ != NULL)
		CloseHandle(poller_);
#else
	if(poller_ != -1)
		close(poller_);
#endif
	if(threads_)
		free(threads_);
	SockSpray *client;
	while(clients_free_.Pop(client) == LNERR_OK)
		delete client;
}

SockPoller *SockPoller::NewInstance(DataBlockPool *pool, LNE_UINT workers, LNE_UINT limit_cache)
{
	LNE_ASSERT(pool != NULL && workers > 0, NULL);
	SockPoller *retval = NULL;
	try {
		retval = new SockPoller(pool, workers, limit_cache);
		if(retval) {
			if(!retval->initialized_) {
				delete retval;
				retval = NULL;
			}
		}
	} catch(std::bad_alloc) {
	}
	return retval;
}

void SockPoller::Release(void)
{
	bool can_destroy = false;
	lock_.Lock();
	can_destroy = --reference_count_ < 1;
	lock_.Unlock();
	if(can_destroy)
		delete this;
}

LNE_UINT SockPoller::Managed(SockStream *stream, SockHander *hander, void *context)
{
	LNE_ASSERT(stream != NULL && stream->Available() && hander != NULL, LNERR_PARAMETER);
	SockSpray *client  = NULL;
	lock_.Lock();
	if(clients_free_.Pop(client) != LNERR_OK) {
		try {
			client = new SockSpray(this, limit_cache_);
		} catch(std::bad_alloc) {
		}
	}
	lock_.Unlock();
	if(client == NULL)
		return LNERR_NOMEMORY;
	// detach stream socket
	client->socket_ = stream->socket_;
	stream->socket_ = INVALID_SOCKET;
	client->hander_ = hander;
	client->context_ = context;
	client->poller_ = poller_;
	LNE_UINT result = client->Apply();
	lock_.Lock();
	if(result == LNERR_OK)
		++reference_count_;
	else if(clients_free_.Push(client) != LNERR_OK)
		delete client;
	lock_.Unlock();
	return result;
}

void SockPoller::FreeSock(SockSpray *client)
{
	LNE_ASSERT2(client != NULL);
	client->Clean();
	bool can_destroy = false;
	lock_.Lock();
	if(clients_free_.Push(client) != LNERR_OK)
		delete client;
	can_destroy = --reference_count_ < 1;
	lock_.Unlock();
	if(can_destroy)
		delete this;
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
	SockSpray::IOCP_OVERLAPPED *overlap;
	do {
		if(GetQueuedCompletionStatus(poller_, &bytes, &key, reinterpret_cast<LPOVERLAPPED *>(&overlap), 500)) {
			if(overlap->type == SockSpray::IOCP_RECV)
				overlap->owner->HandleRecv(pool_);
			else if(overlap->type == SockSpray::IOCP_SEND)
				overlap->owner->HandleSend();
			else
				overlap->owner->HandleShutdown();
		}
	} while(!exit_request_);
}
#elif defined(LNE_LINUX)
void SockPoller::Service(void)
{
	int rc;
	SockSpray *client;
	struct epoll_event event;
	do {
		rc = epoll_wait(poller_, &event, 1, 500);
		if(rc > 0) {
			client = reinterpret_cast<SockSpray *>(event.data.ptr);
			if(event.events & EPOLLIN)
				client->HandleRecv(pool_);
			if(event.events & EPOLLOUT)
				client->HandleSend();
			if(event.events & (EPOLLERR | EPOLLHUP))
				client->HandleShutdown();
		}
	} while(!exit_request_);
}
#elif defined(LNE_FREEBSD)
void SockPoller::Service(void)
{
	int rc;
	SockSpray *client;
	struct timespec timeout;
	struct kevent event, kev;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 500 * 1000000;
	do {
		rc = kevent(poller_, NULL, 0, &event, 1, &timeout);
		if(rc > 0) {
			client = reinterpret_cast<SockSpray *>(event.udata);
			if(event.flags & (EV_EOF | EV_ERROR)) {
				EV_SET(&kev, event.ident, event.filter, EV_DELETE, 0, 0, NULL);
				kevent(poller_, &kev, 1, NULL, 0, NULL);
				client->HandleShutdown();
			} else {
				if(event.filter == EVFILT_READ)
					client->HandleRecv(pool_);
				else if(event.filter == EVFILT_WRITE)
					client->HandleSend();
			}
		}
	} while(!exit_request_);
}
#endif

