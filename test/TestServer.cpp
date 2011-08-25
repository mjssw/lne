#include "test.h"

#include <lne/Thread.h>
#include <lne/DataBlock.h>
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
			printf("Spary accept %s <= %s\n", addr_sock.get_addr_text(), addr_peer.get_addr_text());
		}
		DataBlock *blocks[2];
		LNE_UINT64 index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("Spary [%llu] Recv Data: %u\n", index, block->get_size());
		blocks[0] = block;
		blocks[1] = block;
		client->Send(blocks, 2);
		block->Release();
		if(count_++ > 2)
			client->Shutdown();
	}

	void HandleShutdown(SockSpray *client) {
		LNE_UINT64 index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("Spary [%llu] Shutdown\n", index);
		delete this;
	}

	void HandleTerminate(SockSpray *client) {
		LNE_UINT64 index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("Spary [%llu] Terminate\n", index);
		delete this;
	}
private:
	bool first_;
	LNE_UINT count_;
};

class SpringHandler: public SockSpringHandler
{
public:
	SpringHandler(SockSprayPool *pool, SockReactor *reactor) {
		count_ = 0;
		reactor_ = reactor;
		pool_ = pool;
		pool_->AddRef();
	}
	~SpringHandler() {
		pool_->Release();
	}
	void HandleClient(SockSpring *spring, SockPad client) {
		SockSpray *spray = pool_->Alloc(client, new SprayHandler(), reinterpret_cast<void *>(++count_));
		if(spray)
			reactor_->Bind(spray);
		if(count_ > 2) {
			spring->Shutdown();
			spring->Release();
			printf("Spring Shutdown\n");
			delete this;
		}
	}
	void HandleTerminate(SockSpring *spring) {
		printf("Spring Terminate\n");
		delete this;
	}
private:
	LNE_UINT count_;
	SockReactor *reactor_;
	SockSprayPool *pool_;
};


void TestServer()
{
	SockAddr addr(8080);
	SockPad skpad;
	if(SockAcceptor::NewInstance(skpad, addr) != LNERR_OK) {
		printf("acceptor cannot create\n");
		return;
	}
	SockReactor *reactor = SockReactor::NewInstance(10);
	if(reactor == NULL) {
		printf("reactor cannot create\n");
		return;
	}
	DataBlockPool *data_pool =	DataBlockPool::NewInstance();
	SockSprayPool *spray_pool =	SockSprayPool::NewInstance(data_pool);
	data_pool->Release();
	SockSpringPool *spring_pool = SockSpringPool::NewInstance();
	SockSpring *spring = spring_pool->Alloc(skpad, new SpringHandler(spray_pool, reactor), NULL);
	spray_pool->Release();
	spring_pool->Release();
	reactor->Bind(spring);
	do {
		Thread::Sleep(10000);
	} while(1);
	reactor->Release();
}
