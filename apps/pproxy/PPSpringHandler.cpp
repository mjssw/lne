#include "PPSpringHandler.h"
#include "PPSprayHandler.h"

static PPSprayHandler s_spray_handler;

PPSpringHandler::PPSpringHandler()
	: lock_(true)
{
	count_ = 0;
	pool_ = NULL;
	reactor_ = NULL;
	connector_ = NULL;
}

PPSpringHandler::~PPSpringHandler(void)
{
	if(pool_)
		pool_->Release();
}

void PPSpringHandler::Initialize(SockSprayPool *pool, SockReactor *reactor, SockConnector *connector)
{
	count_ = 0;
	pool_ = pool;
	pool_->AddRef();
	reactor_ = reactor;
	connector_ = connector;
}

void PPSpringHandler::HandleClient(SockSpring *spring, SockPad client)
{
	SockPad child;
	if(connector_->Connect(child) == LNERR_OK) {
		SockSpray *local = pool_->Alloc(client, &s_spray_handler);
		SockSpray *remote = pool_->Alloc(child, &s_spray_handler);
		if(local != NULL && remote != NULL) {
			LNE_UINT index;
			lock_.Lock();
			index = ++count_;
			lock_.Unlock();
			SockAddr addr_sock, addr_peer;
			remote->GetSockAddr(addr_sock);
			local->GetPeerAddr(addr_peer);
			printf("[%u] connect source:(%s), local:(%s)\n", index, addr_peer.addr_text(), addr_sock.addr_text());
			local->set_context(remote);
			remote->set_context(local);
			local->AddRef();
			remote->AddRef();
			reactor_->Bind(local);
			reactor_->Bind(remote);
		} else {
			if(local)
				local->Release();
			if(remote)
				remote->Release();
		}
	}
}

void PPSpringHandler::HandleTerminate(SockSpring *spring)
{
}
