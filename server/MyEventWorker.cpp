#include "MyEventWorker.h"
#include <iostream>

using namespace GG;
using std::cout;
using std::endl;

MyEventWorker::MyEventWorker()
{
}


MyEventWorker::~MyEventWorker()
{
}


int MyEventWorker::MyEventInit()
{
	m_event_base = event_base_new();

	int fds[2];

	if (pipe(fds))
	{
		perror("Can't create notify pipe");
		exit(1);
	}

	m_work_r_fd = fds[0];
	m_work_w_fd = fds[1];

	auto pEvent = new event;
	event_assign(pEvent, m_event_base, m_work_r_fd, EV_READ | EV_PERSIST, pip_callback, this);
	if (event_add(pEvent, nullptr)) {
		fprintf(stderr, "Can't monitor libevent notify pipe\n");
		exit(1);
	}

	return 0;
}

void MyEventWorker::run()
{
	eventLoop();
}

int MyEventWorker::eventLoop()
{
	event_base_dispatch(m_event_base);
	return 0;
}

void MyEventWorker::SetWorkId(int workId)
{
	m_workId = workId;
}

void MyEventWorker::SetEventTimer(MyEventTimer* eventTimer)
{
	m_EventTimer = eventTimer;
}

const int& MyEventWorker::GetWritePipeFd()
{
	return m_work_w_fd;
}

const int& MyEventWorker::getWorkId()
{
	return m_workId;
}

void MyEventWorker::read_callback(struct bufferevent *bev, void *ctx)
{
	char* rBuffer;
	struct evbuffer *wBuffer;

	rBuffer = evbuffer_readline(bev->input);
	if (rBuffer == NULL)
	{
		return;
	}

	wBuffer = evbuffer_new();
	if (wBuffer != NULL) {
		evbuffer_add_printf(wBuffer, "%s\n", rBuffer);
		bufferevent_write_buffer(bev, wBuffer);
		evbuffer_free(wBuffer);
	}

	Client* pClient = static_cast<Client*>(ctx);
	MyEventWorker* pEventWork = static_cast<MyEventWorker*>(pClient->event_work);
	pEventWork->OnReceive(rBuffer);
}

void MyEventWorker::write_callback(struct bufferevent *bev, void *ctx)
{

}

void MyEventWorker::error_callback(struct bufferevent *bev, short what, void *ctx)
{
	struct Client *client = (struct Client*)ctx;
	auto pEventWorker = static_cast<MyEventWorker*>(client->event_work);
	pEventWorker->delClient(client);
}

void MyEventWorker::pip_callback(evutil_socket_t fd, short what, void *ctx)
{
	void* buf[1];
	if (read(fd, buf, sizeof(void*)) != sizeof(void*)) {
		fprintf(stderr, "Can't read from libevent pipe\n");
		return;
	}

	MyEventWorker* pThisObject = static_cast<MyEventWorker*>(ctx);
	pThisObject->addClient(static_cast<Client*>(buf[0]));
}

void MyEventWorker::addEvent(Client* pClient)
{
	m_buf[0] = pClient;
	if (write(GetWritePipeFd(), m_buf, sizeof(void*)) != sizeof(void*)) {
		perror("Writing to thread notify pipe");
		return;
	}
}

void MyEventWorker::addClient(Client* pClient)
{
	pClient->buf_ev = bufferevent_socket_new(m_event_base, pClient->fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(pClient->buf_ev, MyEventWorker::read_callback, MyEventWorker::write_callback, MyEventWorker::error_callback, pClient);
	bufferevent_enable(pClient->buf_ev, EV_READ);

	std::cout << "Event work thread " << getWorkId() << " add connect " << pClient->ipAddr << " : " << pClient->port << std::endl;
	m_ConnectClientTable.insert(std::make_pair(pClient->fd, pClient));
	std::cout << getWorkId() << " m_ConnectClientTable Add " << m_ConnectClientTable.size() << std::endl;
}

void MyEventWorker::delClient(Client* pClient)
{
	m_ConnectClientTable.erase(pClient->fd);
	std::cout << getWorkId() << " m_ConnectClientTable Del " << m_ConnectClientTable.size() << std::endl;
	bufferevent_free(pClient->buf_ev);
	close(pClient->fd);
	delete pClient;
}

void MyEventWorker::OnReceive(char* rBuffer)
{
	if (strcmp(rBuffer, "stop") == 0)
	{
		exit(-1);
	}
	else if (strcmp(rBuffer, "add") == 0)
	{
		auto pTimerInfo = new TimerInfo();
		pTimerInfo->type = TET_ADD;
		pTimerInfo->id = TI_TEST;
		pTimerInfo->loopCounts = 12;
		pTimerInfo->tv.tv_sec = 1;
		pTimerInfo->tv.tv_usec = 0;
		pTimerInfo->cb = MyEventTimer::OnTime;
		pTimerInfo->eventTimer = GetEventTimer();

		GetEventTimer()->AddEvent(pTimerInfo);
	}
	else  if (strcmp(rBuffer, "del") == 0)
	{
		auto pTimerInfo = new TimerInfo();
		pTimerInfo->type = TET_DEL;
		pTimerInfo->id = TI_TEST;
		pTimerInfo->loopCounts = 0;
		pTimerInfo->tv.tv_sec = 0;
		pTimerInfo->tv.tv_usec = 0;
		pTimerInfo->cb = nullptr;
		pTimerInfo->eventTimer = GetEventTimer();

		GetEventTimer()->AddEvent(pTimerInfo);
	}
	cout << rBuffer << endl;
}

void MyEventWorker::OnSend()
{

}

void MyEventWorker::OnError()
{

}