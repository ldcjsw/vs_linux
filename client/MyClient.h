#pragma once
#include "MyThread.h"
#include <stdio.h>  
#include <unistd.h>  
#include <strings.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string> 
#include <iostream>
#include <event.h>
#include <fcntl.h>
#include <event2/bufferevent.h>

namespace GG {
	class MyClient : public MyThread
	{
	public:
		MyClient();
		~MyClient();

		virtual void run();
		void Receive();
		void Send(string str);
		void Time();

	private:
		void clientFun();
		static void eventcb(struct bufferevent *bev, short events, void *arg);
		static void readcb(struct bufferevent *bev, void *arg);
		static void writecb(struct bufferevent *bev, void *arg);
		static void timecb(evutil_socket_t, short, void *);
		int setnonblock(int fd);

	private:
		struct event_base *m_event_base;
		struct bufferevent *m_bev;
		int m_fd;
		struct event m_time;
		string m_str;
	};
}


