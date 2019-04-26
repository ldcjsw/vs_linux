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

#include "MyThread.h"
#include "MyEventTimer.h"
#include "share.h"
#include <list>
#include <unordered_map>

namespace GG {
	class MyEventMultiplexor :
		public MyThread
	{
	public:
		MyEventMultiplexor();
		~MyEventMultiplexor();

		int MyEventInit();
		void addEvent(Client* pClient);
		void SetWorkId(int workId);
		void SetEventTimer(MyEventTimer* eventTimer);
		void StopEventLoop();
		
		MyEventTimer* GetEventTimer() { return m_EventTimer; }
		const int& GetWritePipeFd();
		struct event_base* getWorkerEventBase() { return m_event_base; }

	protected:
		virtual void run();

	private:
		int eventLoop();
		void addClient(Client* pClient);
		void delClient(Client* pClient);

		const int& getWorkId();
		static void pip_callback(evutil_socket_t, short, void *);
		static void read_callback(struct bufferevent *bev, void *ctx);
		static void write_callback(struct bufferevent *bev, void *ctx);
		static void error_callback(struct bufferevent *bev, short what, void *ctx);

	public:
		void OnReceive(Client*);
		void OnSend();
		void OnError();

	private:
		struct event_base* m_event_base;
		MyEventTimer*  m_EventTimer;
		int m_socket_listen;
		int m_workId;
		int m_work_r_fd;
		int m_work_w_fd;
		unordered_map<int, Client*> m_ConnectClientTable;
		void* m_buf[1];
	};
}

