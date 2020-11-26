#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <deque>
#include <future>

class ThreadPool
{
public:
	ThreadPool(int size_t);
	~ThreadPool();
	void enqueue(std::packaged_task<void()> task);
	std::atomic<int> workerdone;

private:
	std::mutex eventmutex;
	std::vector<std::thread> threads;
	
	std::condition_variable cv;
	std::deque <std::packaged_task<void()>> q;
	bool bStop = false;
};