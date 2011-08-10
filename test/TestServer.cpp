#include "test.h"

LNE_NAMESPACE_USING

class MyHander: public SockHander
{
public:
	MyHander() {
		count_ = 0;
	}
	void HandleData(SockPad *client, DataBlock *block) {
		DataBlock *blocks[2];
		LNE_UINT index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("[%u] Recv Data: %u\n", index, block->get_size());
		blocks[0] = block;
		blocks[1] = block;
		client->Send(blocks, 2);
		block->Release();
		if(++count_ > 2)
			client->Shutdown();
	}

	void HandleShutdown(SockPad *client) {
		LNE_UINT index = reinterpret_cast<LNE_UINT64>(client->get_context());
		printf("[%u] Shutdown\n", index);
		delete this;
	}
private:
	LNE_UINT count_;
};

void TestServer()
{
	SockAddr addr(8080);
	SockAcceptor *acceptor = SockAcceptor::NewInstance(addr);
	if(acceptor == NULL) {
		printf("acceptor cannot create\n");
		return;
	}
	DataBlockPool *pool = DataBlockPool::NewInstance();
	SockPoller *poller = SockPoller::NewInstance(pool, 10);
	if(poller == NULL) {
		printf("poller cannot create\n");
		pool->Release();
		acceptor->Release();
		return;
	}
	LNE_UINT count = 0;
	SockStream *stream;
	SockAddr addr_sock, addr_peer;
	while(acceptor->Accept(&stream) == LNERR_OK) {
		if(count > 0) {
			stream->Release();
			break;
		}
		stream->GetSockAddr(addr_sock);
		stream->GetPeerAddr(addr_peer);
		printf("accept %s <= %s\n", addr_sock.get_addr_text(), addr_peer.get_addr_text());
		poller->Managed(stream, new MyHander(), reinterpret_cast<void *>(++count));
		stream->Release();
	}
	pool->Release();
	poller->Release();
	acceptor->Release();
}
