#include "MyEventTimer.h"

using namespace GG;


MyEventTimer::MyEventTimer()
{
}


MyEventTimer::~MyEventTimer()
{
}

void MyEventTimer::MyEventInit()
{
	m_event_base = event_base_new();
	int fds[2];

	if (pipe(fds))
	{
		perror("Can't create notify pipe");
		exit(1);
	}

	m_time_r_fd = fds[0];
	m_time_w_fd = fds[1];

	auto pEvent = new event;
	event_assign(pEvent, m_event_base, m_time_r_fd, EV_READ | EV_PERSIST, pip_callback, this);
	if (event_add(pEvent, nullptr)) {
		fprintf(stderr, "Can't monitor libevent notify pipe\n");
		exit(1);
	}
}

const int& MyEventTimer::GetWritePipeFd()
{
	return m_time_w_fd;
}

void MyEventTimer::pip_callback(evutil_socket_t fd, short what, void *ctx)
{
	void* buf[1];
	if (read(fd, buf, sizeof(void*)) != sizeof(void*)) {
		fprintf(stderr, "Can't read from libevent pipe\n");
		return;
	}

	MyEventTimer* pThisObject = static_cast<MyEventTimer*>(ctx);
	auto pTimerInfo = static_cast<TimerInfo*>(buf[0]);
	switch (pTimerInfo->type)
	{
	case TET_ADD:
		pThisObject->AddTimer(pTimerInfo);
		break;
	case TET_DEL:
		pThisObject->DelTimer(pTimerInfo);
		break;
	case TET_MODIFY:
		pThisObject->ModifyTimer(pTimerInfo);
		break;
	default:
		break;
	}
}

void MyEventTimer::OnTime(int sock, short event, void *arg)
{
	if (arg != nullptr)
	{
		auto pTimerInfo = static_cast<TimerInfo*>(arg);

		std::cout << pTimerInfo->loopCounts << std::endl;

		if (pTimerInfo->loopCounts > 0)
		{
			pTimerInfo->loopCounts--;
		}

		if (pTimerInfo->loopCounts == 0)
		{
			auto pEventTime = static_cast<MyEventTimer*>(pTimerInfo->eventTimer);
			pEventTime->DelTimer(pTimerInfo);
		}
	}

}

int MyEventTimer::eventLoop()
{
	event_base_dispatch(m_event_base);
	return 0;
}

void MyEventTimer::AddEvent(TimerInfo* pTimerInfo)
{
	m_buf[0] = pTimerInfo;
	if (write(GetWritePipeFd(), m_buf, sizeof(void*)) != sizeof(void*)) {
		perror("Writing to thread notify pipe");
	}
}

void MyEventTimer::AddTimer(TimerInfo* pTimerInfo)
{
	auto it = m_TimerInfoTable.find(pTimerInfo->id);
	if (it != m_TimerInfoTable.end()) {
		delete pTimerInfo;
		return;
	}

	event_assign(&(pTimerInfo->tEvent), m_event_base, -1, EV_PERSIST, pTimerInfo->cb, pTimerInfo);
	evtimer_add(&(pTimerInfo->tEvent), &(pTimerInfo->tv));
	m_TimerInfoTable.insert(std::make_pair(pTimerInfo->id, pTimerInfo));
}

void MyEventTimer::DelTimer(TimerInfo* pTimerInfo)
{
	auto it = m_TimerInfoTable.find(pTimerInfo->id);
	if (it != m_TimerInfoTable.end())
	{
		TimerInfo* timerInfo = it->second;
		evtimer_del(&(timerInfo->tEvent));
		m_TimerInfoTable.erase(it);
	}
	delete pTimerInfo;
}

void MyEventTimer::ModifyTimer(TimerInfo* pTimerInfo)
{

}

void MyEventTimer::run()
{
	eventLoop();
}
