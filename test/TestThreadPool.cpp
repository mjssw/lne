#include "test.h"

LNE_NAMESPACE_USING

class MyTask: public ThreadTask
{
public:
	MyTask(LNE_UINT id) {
		id_ = id;
	}

	void Service(void) {
		lock_.Lock();
		std::cout << "Id: " << id_ << " Run" << std::endl;
		lock_.Unlock();
		Thread::Sleep((rand() % 100) + 100);
	}

	void Discard(void) {
		lock_.Lock();
		std::cout << "Id: " << id_ << " Discard" << std::endl;
		lock_.Unlock();
		delete this;
	}

private:
	LNE_UINT id_;
	static ThreadLock lock_;
};

ThreadLock MyTask::lock_(true);

void TestThreadPool()
{
	ThreadPool *pool = ThreadPool::NewInstance(200);
	for(int i = 0; i < 100000; i++) {
		pool->PostTask(new MyTask(i + 1));
	}
	Thread::Sleep(10 * 1000);
	pool->Release();
}

