#include "test.h"

LNE_NAMESPACE_USING

void TestClient()
{
	TimeValue tv(3);
	SockAddr addr("www.google.com:80");
	SockConnector *connector = SockConnector::NewInstance(addr, &tv);
	if(connector == NULL) {
		printf("connector cannot create\n");
		return;
	}
	SockWaves *stream;
	if(connector->Connect(&stream) != LNERR_OK) {
		printf("connector cannot connect\n");
		connector->Release();
		return;
	}
	const char *query = "GET / HTTP/1.1\r\n\r\n";
	DataBlock *block = DataBlock::NewInstance(1024 * 1024);
	strcpy(block->get_buffer(), query);
	block->set_size(strlen(block->get_buffer()));
	stream->Send(block);
	while(stream->Recv(block, tv) == LNERR_OK) {
		block->get_buffer()[block->get_size()] = '\0';
		puts(block->get_buffer());
	}
	block->Release();
	SockAddr addr_sock, addr_peer;
	stream->GetSockAddr(addr_sock);
	stream->GetPeerAddr(addr_peer);
	printf("connect %s => %s\n", addr_sock.get_addr_text(), addr_peer.get_addr_text());
	stream->Release();
	connector->Release();
}
