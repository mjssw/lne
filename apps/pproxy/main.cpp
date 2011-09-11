#include <stdio.h>
#include <string.h>
#include <lne/Thread.h>
#include <lne/SockSpray.h>
#include <lne/SockSpring.h>
#include <lne/SockReactor.h>
#include <lne/SockAcceptor.h>
#include <lne/SockConnector.h>
#include "PPSpringHandler.h"

LNE_NAMESPACE_USING

int main(int argc, char *argv[])
{
	if(argc < 3 || argc > 5) {
		printf("Usage: <port> <remote_addr> [remote_port] [-6]\n");
		return -1;
	}
	int address_family = AF_INET;
	int local_port = atoi(argv[1]);
	int remote_port = local_port;
	if(argc == 4)
		remote_port = atoi(argv[3]);
	if(argc == 5) {
		if(strcmp(argv[4], "-6") == 0)
			address_family = AF_INET6;
	}
	SockPad skpad;
	SockAddr local_addr, remote_addr;
	local_addr.Set(local_port, address_family);
	remote_addr.Set(remote_port, argv[2], address_family);
	PPSpringHandler spring_handler;
	if(SockAcceptor::NewInstance(skpad, local_addr) != LNERR_OK) {
		printf("SockAcceptor cannot create [%s]\n", local_addr.addr_text());
		return -1;
	}
	AutoRelease<DataBlockPool> data_pool(DataBlockPool::NewInstance());
	if(!data_pool) {
		printf("DataBlockPool cannot create \n");
		return -1;
	}
	AutoRelease<SockSprayPool> spray_pool(SockSprayPool::NewInstance(data_pool));
	if(!spray_pool) {
		printf("SockSprayPool cannot create \n");
		return -1;
	}
	AutoRelease<SockSpringPool> spring_pool(SockSpringPool::NewInstance());
	if(!spring_pool) {
		printf("SockSprayPool cannot create \n");
		return -1;
	}
	AutoRelease<SockConnector> connector(SockConnector::NewInstance(remote_addr));
	if(!connector) {
		printf("SockConnector cannot create [%s]\n", remote_addr.addr_text());
		return -1;
	}
	AutoRelease<SockReactor>reactor(SockReactor::NewInstance(10, 5));
	if(!reactor) {
		printf("SockReactor cannot create\n");
		return -1;
	}
	spring_handler.Initialize(spray_pool, reactor, connector);
	SockSpring *spring = spring_pool->Alloc(skpad, &spring_handler);
	if(spring == NULL) {
		printf("SockSpring cannot create\n");
		return -1;
	}
	reactor->Bind(spring);
	do {
		Thread::Sleep(10000);
	} while(true);
	return 0;
}
