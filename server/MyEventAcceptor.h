#pragma once
#include <event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <event2/listener.h>
#include <iostream>

#include "MyThread.h"
#include "MyEventMultiplexor.h"
#include <vector>

namespace GG {
	class MyEventAcceptor :
		public MyThread
	{
	public:
		MyEventAcceptor();
		~MyEventAcceptor();

		int MyEventInit();
		void SetEventWorks(vector<MyEventMultiplexor*>);
		void SetEventTimer(MyEventTimer*);
		void StopServer();
		void StopEventLoop();

	protected:
		virtual void run();

	private:
		int acceptorSocketInit();
		int setnonblock(int fd);
		int setClientSocket();
		int eventLoop();
		MyEventMultiplexor* getOneEventWork();
		static void accept_callback(evutil_socket_t, short, void *);
		static void signall_callback(int fd, short event, void *arg);

	private:
		struct event_base* m_event_base;
		struct event m_accept_event;
		struct evconnlistener* m_listener;
		struct event m_signal_event;
		int m_socket_listen;
		vector<MyEventMultiplexor*> m_EventMultiplexor;
		MyEventTimer* m_EventTimer;
		size_t m_worksLoopFlag;
	};
}
