#include "test.h"

#include <lne/Thread.h>
#include <lne/ThreadMutex.h>
#include <lne/ThreadSemaphore.h>

LNE_NAMESPACE_USING

class MyRunner: public Runnable
{
public:
	MyRunner(LNE_UINT id) {
		id_ = id;
		running_ = true;
	}

	void Service(void) {
		LNE_UINT index;
		while(running_) {
			mutex_.Acquire();
			index = ++index_;
			std::cout << "Id: " << id_ << " Index: " << index << std::endl;
			mutex_.Release();
			Thread::Sleep(id_ * (rand() % 100) + 100);
		}
	}

	void Terminate(void) {
		running_ = false;
	}

private:
	LNE_UINT id_;
	bool running_;
	static LNE_UINT index_;
	static ThreadMutex mutex_;
};

LNE_UINT MyRunner::index_ = 0;
ThreadMutex MyRunner::mutex_;

void TestThread()
{
	MyRunner run1(1);
	MyRunner run2(10);
	Thread *thread1, *thread2;
	TimeValue tv(10, 500);
	ThreadSemaphore sleeper(0);
	srand((unsigned int)time(NULL));
	thread1 = Thread::NewInstance(&run1);
	thread2 = Thread::NewInstance(&run2);
	thread1->Active();
	thread2->Active();
	sleeper.Acquire(tv);
	thread1->Wait();
	thread2->Wait();
}
