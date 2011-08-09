#include "test.h"

int main(int argc, char *argv[])
{
	if(argc < 2) {
		std::cout << "Need Parameter" << std::endl;
		return -1;
	}
	srand(static_cast<unsigned int>(time(NULL)));
	if(strcmp(argv[1], "thread") == 0) {
		TestThread();
	} else if(strcmp(argv[1], "threadpool") == 0) {
		TestThreadPool();
	} else if(strcmp(argv[1], "client") == 0) {
		TestClient();
	} else if(strcmp(argv[1], "server") == 0) {
		TestServer();
	} else if(strcmp(argv[1], "object") == 0) {
		TestObject();
	} else {
		std::cout << "Not support" << std::endl;
		return -1;
	}
	std::cout << "Programe Exit" << std::endl;
	getchar();
}
