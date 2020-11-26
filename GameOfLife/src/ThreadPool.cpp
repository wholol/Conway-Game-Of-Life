#include "ThreadPool.h"
#include "Timer.h"

ThreadPool::ThreadPool(int size_t)
{
	for (int i = 0; i < size_t; ++i)
	{
		threads.emplace_back(
			[=]{
			while (true)
			{

				std::packaged_task<void()> t;
				{
					std::unique_lock<std::mutex> m(eventmutex);

					cv.wait(m, [=] {return bStop || !q.empty(); });

					if (bStop && q.empty()) break;

					t = std::move(q.front());

					q.pop_front();
				}
				t();
				++workerdone;
			}
		});
	}
}


ThreadPool::~ThreadPool()
{
	bStop = true;
	cv.notify_all();
	for (auto& t : threads)
	{
		t.join();
	}
}

void ThreadPool::enqueue(std::packaged_task<void()> task)
{
	{
		std::unique_lock<std::mutex> m(eventmutex);
		q.push_back(std::move(task));
	}
	cv.notify_one();
}
