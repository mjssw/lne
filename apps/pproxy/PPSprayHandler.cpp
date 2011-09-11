#include "PPSprayHandler.h"

PPSprayHandler::PPSprayHandler(void)
{
}

PPSprayHandler::~PPSprayHandler(void)
{
}

void PPSprayHandler::HandleData(SockSpray *client, DataBlock *block)
{
	SockSpray *pair = static_cast<SockSpray *>(client->context());
	pair->Send(block);
}

void PPSprayHandler::HandleShutdown(SockSpray *client)
{
	SockAddr addr_sock, addr_peer;
	client->GetSockAddr(addr_sock);
	client->GetPeerAddr(addr_peer);
	printf("  [%p] shutdown local:(%s), remote:(%s)\n", client, addr_sock.addr_text(), addr_peer.addr_text());
	SockSpray *pair = static_cast<SockSpray *>(client->context());
	if(pair != NULL) {
		client->set_context(NULL);
		pair->Shutdown();
		pair->Release();
	}
}

void PPSprayHandler::HandleTerminate(SockSpray *client)
{
	SockSpray *pair = static_cast<SockSpray *>(client->context());
	if(pair != NULL) {
		client->set_context(NULL);
		pair->Release();
	}
}
