#include "MyThread.h"
#include <string.h>

using namespace GG;

MyThread::MyThread(bool joinable)
{
	memset(&m_thread, 0, sizeof(m_thread));
	m_joinable = joinable;
}


MyThread::~MyThread()
{
}

int MyThread::start()
{
	int err = pthread_create(&m_thread, NULL, MyThread::threadEntry, this);
	if (err == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void MyThread::Join()
{
	pthread_join(m_thread, NULL);
}

bool MyThread::Joinbale()
{
	return m_joinable;
}

void MyThread::SetJoinAble(bool joinable)
{
	m_joinable = joinable;
}

void MyThread::run()
{

}

void* MyThread::threadEntry(void *param)
{
	MyThread* pThread = static_cast<MyThread*>(param);
	if (pThread != nullptr)
	{
		if (pThread->Joinbale() == false)
		{
			pthread_detach(pthread_self());
		}

		pThread->run();
	}
	else
	{
		pthread_detach(pthread_self());
	}

	return NULL;
}
