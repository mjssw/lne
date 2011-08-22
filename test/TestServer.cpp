#include "test.h"

#include <lne/Thread.h>
#include <lne/DataBlockPool.h>
#include <lne/SockAcceptor.h>
#include <lne/SockSpray.h>
#include <lne/SockSpring.h>
#include <lne/SockReactor.h>

LNE_NAMESPACE_USING

class SprayHandler: public SockSprayHandler
{
public:
	SprayHandler() {
		count_ = 0;
		first_ = false;
	}
	void HandleData(SockSpray *client, DataBlock *block) {
		if(!first_) {
			first_ = true;
			SockAddr addr_sock, addr_peer;
			client->GetSockAddr(addr_sock);
			client->GetPeerAddr(addr_peer);
			printf("accept %s <= %s\n", addr_sock.get_addr_text(), addr_peer.get_addr_text());
		}
		DataBlock *blocks[2];
		LNE_UINT64 index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("[%llu] Recv Data: %u\n", index, block->get_size());
		blocks[0] = block;
		blocks[1] = block;
		client->Send(blocks, 2);
		block->Release();
		if(++count_ > 2)
			client->Shutdown();
	}

	void HandleShutdown(SockSpray *client) {
		LNE_UINT64 index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("[%llu] Shutdown\n", index);
		delete this;
	}
private:
	bool first_;
	LNE_UINT count_;
};

class SpringHandler: public SockSpringHandler
{
public:
	SpringHandler(SockSprayFactory *factory, SockReactor *poller) {
		count_ = 0;
		poller_ = poller;
		factory_ = factory;
		factory_->AddRef();
	}
	~SpringHandler() {
		factory_->Release();
	}
	void HandleClient(SockSpring *spring, SockPad client) {
		SockSpray *spray = factory_->Alloc(client, new SprayHandler(), reinterpret_cast<void *>(++count_));
		if(spray)
			poller_->Bind(spray);
	}
	void HandleShutdown(SockSpring *spring) {
		delete this;
	}
private:
	LNE_UINT count_;
	SockReactor *poller_;
	SockSprayFactory *factory_;
};


void TestServer()
{
	SockAddr addr(8080);
	SockPad skpad;
	if(SockAcceptor::NewInstance(skpad, addr) != LNERR_OK) {
		printf("acceptor cannot create\n");
		return;
	}
	SockReactor *poller = SockReactor::NewInstance(10, 10);
	if(poller == NULL) {
		printf("poller cannot create\n");
		return;
	}
	DataBlockPool *pool =	DataBlockPool::NewInstance();
	SockSprayFactory *ssfactory =	SockSprayFactory::NewInstance(pool);
	pool->Release();
	SockSpringFactory *factory = SockSpringFactory::NewInstance();
	SockSpring *spring = factory->Alloc(skpad, new SpringHandler(ssfactory, poller), NULL);
	ssfactory->Release();
	factory->Release();
	poller->Bind(spring);
	Thread::Sleep(10000);
	poller->Release();
}
