#include "MyClient.h"

using namespace GG;
using std::cout;
using std::endl;

#define HOST "106.14.14.142"  
#define PORT 9527  
#define MAXDATASIZE 1000 

struct NetData
{
	size_t datalen;
	void* data;
	NetData() {
		datalen = 0;
		data = nullptr;
	};

	~NetData() {
		delete data;
	}
};

struct MyStruct
{
	int a = 0;
	int b = 1;
};

MyClient::MyClient()
{
}


MyClient::~MyClient()
{
}

void MyClient::run()
{
	clientFun();
}

void MyClient::eventcb(struct bufferevent *bev, short events, void *arg)
{
	if (events & BEV_EVENT_CONNECTED)
	{
		auto pThis = static_cast<MyClient*>(arg);
		pThis->Send("start 1");
	}
	else if (events & BEV_EVENT_ERROR)
	{
		perror("An error occured while connecting.\n");
	}

	return;
}

void MyClient::readcb(struct bufferevent *bev, void *arg)
{
	auto pThis = static_cast<MyClient*>(arg);
	pThis->Receive();
}

void MyClient::Receive()
{
	struct evbuffer *input = bufferevent_get_input(m_bev);

	size_t datalen = 0;
	evbuffer_remove(input, &datalen, sizeof(datalen));
	char* buf = new char[datalen + 1];
	evbuffer_remove(input, buf, datalen);
	buf[datalen] = '\0';
	if (datalen != strlen(buf))
	{
		cout << datalen << " :"<< buf << endl;
	}
	else
	{
		cout << datalen << " ";
	}

	if (datalen == 1000)
	{
		event_base_loopexit(m_event_base, nullptr);
	}

	delete buf;
}

void MyClient::writecb(struct bufferevent *bev, void *arg)
{

}

void MyClient::Send(string str)
{
	struct evbuffer *output = bufferevent_get_output(m_bev);
	size_t sizeBuf = str.size();
	void* buf = new char[str.size() + sizeof(size_t)];
	memcpy(buf, &sizeBuf, sizeof(size_t));
	memcpy(buf + sizeof(size_t), str.c_str(), str.size());

	evbuffer_add(output, buf, str.size() + sizeof(size_t));
}

void MyClient::timecb(evutil_socket_t fd, short event, void * arg)
{
	auto pThis = static_cast<MyClient*>(arg);
	pThis->Time();
}

void MyClient::Time()
{
	m_str += "0";
	Send(m_str);
}

void MyClient::clientFun()
{
	struct sockaddr_in server;

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr(HOST);
	
	m_event_base = event_base_new();
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	m_bev = bufferevent_socket_new(m_event_base, m_fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(m_bev, MyClient::readcb, MyClient::writecb, MyClient::eventcb, this);

	if (bufferevent_socket_connect(m_bev, (struct sockaddr *)&server, sizeof(struct sockaddr)) != 0)
	{
		perror("connect error");
		event_base_free(m_event_base);
		return;
	}

	setnonblock(m_fd);

	bufferevent_enable(m_bev, EV_READ | EV_WRITE);
	event_assign(&m_time, m_event_base, -1, EV_PERSIST, timecb, this);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;
	evtimer_add(&m_time, &tv);

	event_base_dispatch(m_event_base);
	event_base_free(m_event_base);
}


int MyClient::setnonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);

	return 0;
}