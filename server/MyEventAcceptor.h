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
#include <iostream>

#include "MyThread.h"
#include "MyEventMultiplexor.h"
#include <vector>

using std::vector;

namespace GG {
	class MyEventAcceptor :
		public MyThread
	{
	public:
		MyEventAcceptor();
		~MyEventAcceptor();

		int MyEventInit();
		void SetEventWorks(vector<MyEventMultiplexor*>);

	protected:
		virtual void run();

	private:
		int acceptorSocketInit();
		int setnonblock(int fd);
		int setClientSocket();
		int eventLoop();
		MyEventMultiplexor* getOneEventWork();
		static void accept_callback(int fd, short ev, void *arg);

	private:
		struct event_base* m_event_base;
		struct event m_accept_event;
		int m_socket_listen;
		vector<MyEventMultiplexor*> m_works;
		size_t m_worksLoopFlag;
	};
}
