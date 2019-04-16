#include "MyEventAcceptor.h"
#include "share.h"

using namespace GG;

MyEventAcceptor::MyEventAcceptor()
{
	m_event_base = NULL;
	m_worksLoopFlag = 0;
}

MyEventAcceptor::~MyEventAcceptor()
{
}

struct client
{
	int fd;
	struct bufferevent *buf_ev;
};

int MyEventAcceptor::MyEventInit()
{
	m_event_base = event_base_new();
	acceptorSocketInit();

	event_assign(&m_accept_event, m_event_base, m_socket_listen, EV_READ | EV_PERSIST, MyEventAcceptor::accept_callback, this);
	event_add(&m_accept_event, NULL);

	return 0;
}

void MyEventAcceptor::SetEventWorks(vector<MyEventMultiplexor*> works)
{
	m_works = works;
}

MyEventMultiplexor* MyEventAcceptor::getOneEventWork()
{
	if (m_worksLoopFlag >= m_works.size())
	{
		m_worksLoopFlag = 0;
	}

	return m_works[m_worksLoopFlag++];
}

void MyEventAcceptor::run()
{
	eventLoop();
}

int MyEventAcceptor::eventLoop()
{
	event_base_dispatch(m_event_base);

	return 0;
}

int MyEventAcceptor::acceptorSocketInit()
{
	struct sockaddr_in addresslisten;
	int reuse = 1;

	m_socket_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_listen < 0)
	{
		fprintf(stderr, "Failed to create listen socket");
		return 1;
	}

	memset(&addresslisten, 0, sizeof(addresslisten));

	addresslisten.sin_family = AF_INET;
	addresslisten.sin_addr.s_addr = INADDR_ANY;
	addresslisten.sin_port = htons(SERVER_PORT);

	if (bind(m_socket_listen, (struct sockaddr*)&addresslisten, sizeof(addresslisten)) < 0)
	{
		fprintf(stderr, "Failed to bind");
		return 1;
	}

	if (listen(m_socket_listen, 5) < 0)
	{
		fprintf(stderr, "Failed to listen to socket");
		return 1;
	}

	setsockopt(m_socket_listen, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	setnonblock(m_socket_listen);

	return 0;
}

int MyEventAcceptor::setnonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);

	return 0;
}

void MyEventAcceptor::accept_callback(int fd, short ev, void *arg)
{
	if (arg == nullptr)
	{
		return;
	}

	auto pAcceptor = static_cast<MyEventAcceptor*>(arg);
	pAcceptor->setClientSocket();
}

int MyEventAcceptor::setClientSocket()
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	client_fd = accept(m_socket_listen, (struct sockaddr*)&client_addr, &client_len);

	if (client_fd < 0)
	{
		perror("Client: accept() failed");
		return -1;
	}

	setnonblock(client_fd);

	auto pEventWork = getOneEventWork();
	auto pClient = (struct Client *)calloc(1, sizeof(struct Client));
	if (pClient == NULL)
	{
		perror("1 malloc failed");
		return -1;
	}

	pClient->fd = client_fd;
	pClient->port = client_addr.sin_port;
	char* ipaddr = inet_ntoa(client_addr.sin_addr);
	memcpy(pClient->ipAddr, ipaddr, strlen(ipaddr));
	pClient->event_work = pEventWork;

	pEventWork->addEvent(pClient);

	return 0;
}