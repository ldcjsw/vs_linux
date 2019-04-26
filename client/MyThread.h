#pragma once

#include <pthread.h>
#include <string>

using std::string;

namespace GG {
	class MyThread
	{
	public:
		MyThread(bool joinable = true);
		~MyThread();

	public:
		int start();
		void join();
		bool joinbale();

	protected:
		virtual void run();

	private:
		static void* threadEntry(void *param);

	private:
		pthread_t m_thread;
		bool m_joinable;
	};
}


