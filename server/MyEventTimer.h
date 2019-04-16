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
#include <unordered_map>
#include "MyThread.h"
#include "share.h"

namespace GG {

	enum TimeEventType
	{
		TET_ADD,
		TET_DEL,
		TET_MODIFY,
	};

	enum TimeId
	{
		TI_TEST,
	};

	struct TimerInfo
	{
		int type;
		int id;
		int loopCounts;
		timeval tv;
		struct event tEvent;
		event_callback_fn cb;
		void* eventTimer;

		TimerInfo() {
			type = 0;
			id = 0;
			loopCounts = 0;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			eventTimer = nullptr;
		}
	};

	class MyEventTimer :
		public MyThread
	{
	public:
		MyEventTimer();
		~MyEventTimer();

		void MyEventInit();
		const int& GetWritePipeFd();
		void AddEvent(TimerInfo*);
		static void OnTime(int sock, short event, void *arg);
		void StopEventLoop();

	protected:
		virtual void run();

	private:
		void AddTimer(TimerInfo* pTimerInfo);
		void DelTimer(TimerInfo* pTimerInfo);
		void ModifyTimer(TimerInfo* pTimerInfo);

	private:
		static void pip_callback(evutil_socket_t, short, void *);
		int eventLoop();

	private:
		struct event_base* m_event_base;
		int m_time_r_fd;
		int m_time_w_fd;
		void* m_buf[1];
		unordered_map<int, TimerInfo*> m_TimerInfoTable;
	};
}


