#pragma once

#include <pthread.h>
#include <string>

using std::string;

namespace GG {
	class MyThread
	{
	public:
		MyThread(bool joinable = false);
		~MyThread();

	public:
		int start();
		void Join();
		bool Joinbale();
		void SetJoinAble(bool joinable);

	protected:
		virtual void run();

	private:
		static void* threadEntry(void *param);

	private:
		pthread_t m_thread;
		bool m_joinable;
	};
}


