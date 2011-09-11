#ifndef PPSPRAYHANDLER_H
#define PPSPRAYHANDLER_H

#include <lne/SockSpray.h>

LNE_NAMESPACE_USING

class PPSprayHandler :
	public SockSprayHandler
{
public:
	PPSprayHandler(void);
	~PPSprayHandler(void);

	void HandleData(SockSpray *client, DataBlock *block);
	void HandleShutdown(SockSpray *client);
	void HandleTerminate(SockSpray *client);

};

#endif
