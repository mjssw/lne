#ifndef PPSPRINGHANDLER_H
#define PPSPRINGHANDLER_H

#include <lne/ThreadLock.h>
#include <lne/SockSpring.h>
#include <lne/SockSpray.h>
#include <lne/SockReactor.h>
#include <lne/SockConnector.h>

LNE_NAMESPACE_USING

class PPSpringHandler :
	public SockSpringHandler
{
public:
	PPSpringHandler();
	~PPSpringHandler(void);

	void Initialize(SockSprayPool *pool, SockReactor *reactor, SockConnector *connector);
	void HandleClient(SockSpring *spring, SockPad client);
	void HandleTerminate(SockSpring *spring);

private:
	LNE_UINT count_;
	ThreadLock lock_;
	SockReactor *reactor_;
	SockSprayPool *pool_;
	SockConnector *connector_;
};

#endif
