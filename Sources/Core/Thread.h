#pragma once

#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<memory>

namespace FOCUS
{
	class Thread
	{
	private:
		bool _destroying = false;
		std::thread _worker;
		std::queue<std::function<void()>> _jobQueue;
		std::mutex _queueMutex;
		std::condition_variable _condition;

		void queueTraverse()
		{
			while (true)
			{
				std::function<void()> job;
				
				std::unique_lock<std::mutex> lock(_queueMutex);
				_condition.wait(lock, [this] {
					return !_jobQueue.empty() || _destroying;
				});

				if (_destroying)
				{
					break;
				}

				// excute job
				job = _jobQueue.front();
				job();

				std::lock_guard<std::mutex> lock(_queueMutex);
				_jobQueue.pop();
				_condition.notify_one();
			}
		}

	public:
		Thread()
		{
			_worker = std::thread(&Thread::queueTraverse, this);
		}

		~Thread()
		{
			if (_worker.joinable())
			{
				wait();
				_queueMutex.lock();
				_destroying = true;
				_condition.notify_one();
				_queueMutex.unlock();
				_worker.join();
			}
		}

		void addJob(std::function<void()> job)
		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			_jobQueue.push(std::move(job));
			_condition.notify_one();
		}

		void wait()
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_condition.wait(lock, [this] {
				return _jobQueue.empty();
			});
		}
	};

	


	class ThreadPool
	{
	public:
		std::vector<std::unique_ptr<Thread>> _threads;

		ThreadPool() = default;

		void setThreadCount(uint32_t count)
		{
			_threads.clear();
			for (uint32_t i = 0; i < count; ++i)
			{
				_threads.push_back(std::make_unique<Thread>());
			}
		}

		void wait()
		{
			for (auto& thread : _threads)
			{
				thread->wait();
			}
		}
	};
}