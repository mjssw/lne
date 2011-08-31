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
		SockSpray *in = pool_->Alloc(client, &s_spray_handler);
		SockSpray *out = pool_->Alloc(child, &s_spray_handler);
		if(in != NULL && out != NULL) {
			LNE_UINT index;
			lock_.Lock();
			index = ++count_;
			lock_.Unlock();
			SockAddr addr_sock, addr_peer;
			in->GetPeerAddr(addr_peer);
			out->GetSockAddr(addr_sock);
			printf("[%u] connect in:[%p](%s), out:[%p](%s)\n", index, in, addr_peer.addr_text(), out, addr_sock.addr_text());
			in->set_context(out);
			out->set_context(in);
			in->AddRef();
			out->AddRef();
			reactor_->Bind(in);
			reactor_->Bind(out);
		} else {
			if(in)
				in->Release();
			if(out)
				out->Release();
		}
	}
}

void PPSpringHandler::HandleTerminate(SockSpring *spring)
{
}
