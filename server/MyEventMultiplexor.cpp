#include "MyEventMultiplexor.h"
#include <iostream>

using namespace GG;
using std::cout;
using std::endl;

#define TEST_BUMS 1024

MyEventMultiplexor::MyEventMultiplexor()
{
}


MyEventMultiplexor::~MyEventMultiplexor()
{
}


int MyEventMultiplexor::MyEventInit()
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

void MyEventMultiplexor::run()
{
	eventLoop();
}

int MyEventMultiplexor::eventLoop()
{
	event_base_dispatch(m_event_base);
	return 0;
}

void MyEventMultiplexor::StopEventLoop()
{
	cout << "MyEventMultiplexor::StopEventLoop() " << getWorkId() << endl;
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	event_base_loopexit(m_event_base, &tv);
	//event_base_loopbreak(m_event_base);
}

void MyEventMultiplexor::SetWorkId(int workId)
{
	m_workId = workId;
}

void MyEventMultiplexor::SetEventTimer(MyEventTimer* eventTimer)
{
	m_EventTimer = eventTimer;
}

const int& MyEventMultiplexor::GetWritePipeFd()
{
	return m_work_w_fd;
}

const int& MyEventMultiplexor::getWorkId()
{
	return m_workId;
}

void MyEventMultiplexor::read_callback(struct bufferevent *bev, void *ctx)
{

	Client* pClient = static_cast<Client*>(ctx);
	MyEventMultiplexor* pEventWork = static_cast<MyEventMultiplexor*>(pClient->event_work);
	pEventWork->OnReceive(pClient);
}

void MyEventMultiplexor::write_callback(struct bufferevent *bev, void *ctx)
{
	Client* pClient = static_cast<Client*>(ctx);
	MyEventMultiplexor* pEventWork = static_cast<MyEventMultiplexor*>(pClient->event_work);
	pEventWork->OnSend();
}

void MyEventMultiplexor::error_callback(struct bufferevent *bev, short what, void *ctx)
{
	struct Client *client = (struct Client*)ctx;
	auto pEventWorker = static_cast<MyEventMultiplexor*>(client->event_work);
	pEventWorker->delClient(client);
}

void MyEventMultiplexor::pip_callback(evutil_socket_t fd, short what, void *ctx)
{
	void* buf[1];
	if (read(fd, buf, sizeof(void*)) != sizeof(void*)) {
		fprintf(stderr, "Can't read from libevent pipe\n");
		return;
	}

	MyEventMultiplexor* pThisObject = static_cast<MyEventMultiplexor*>(ctx);
	pThisObject->addClient(static_cast<Client*>(buf[0]));
}

void MyEventMultiplexor::addEvent(Client* pClient)
{
	m_buf[0] = pClient;

	if (write(GetWritePipeFd(), m_buf, sizeof(void*)) != sizeof(void*)) {
		perror("Writing to thread notify pipe");
		return;
	}
}

void MyEventMultiplexor::addClient(Client* pClient)
{
	pClient->buf_ev = bufferevent_socket_new(m_event_base, pClient->fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(pClient->buf_ev, MyEventMultiplexor::read_callback, MyEventMultiplexor::write_callback, MyEventMultiplexor::error_callback, pClient);
	bufferevent_enable(pClient->buf_ev, EV_READ);

	std::cout << "Event work thread " << getWorkId() << " add connect " << pClient->ipAddr << " : " << pClient->port << std::endl;
	m_ConnectClientTable.insert(std::make_pair(pClient->fd, pClient));
	std::cout << getWorkId() << " m_ConnectClientTable Add " << m_ConnectClientTable.size() << std::endl;
}

void MyEventMultiplexor::delClient(Client* pClient)
{
	m_ConnectClientTable.erase(pClient->fd);
	std::cout << getWorkId() << " m_ConnectClientTable Del " << m_ConnectClientTable.size() << std::endl;
	bufferevent_free(pClient->buf_ev);
	delete pClient;
}

void MyEventMultiplexor::OnReceive(Client* pClient)
{

	struct evbuffer *input = bufferevent_get_input(pClient->buf_ev);
	struct evbuffer *output = bufferevent_get_output(pClient->buf_ev);

	evbuffer_add_buffer(output, input);

	/*size_t datalen = 0;
	evbuffer_remove(input, &datalen, sizeof(datalen));
	void* buf = new char[datalen];
	evbuffer_remove(input, buf, datalen);

	auto pData = static_cast<MyStruct*>(buf);
	cout << pData->a << endl;
	cout << pData->b << endl;

	evbuffer_add(output, buf, datalen);

	delete buf;*/
}

void MyEventMultiplexor::OnSend()
{
}

void MyEventMultiplexor::OnError()
{

}