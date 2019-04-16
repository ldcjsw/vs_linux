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

namespace GG {
	class MyEvent : public MyThread
	{
	public:
		MyEvent();
		~MyEvent();

		void MyEventInit();

		int StartEvent();
		int EventTimerLoop();

	protected:
		virtual void run();

	private:
		int AddTimer(int loopCounts, long sec, long usec, event_callback_fn cb);
		void DelTimer(event* pEvent);

	private:
		static void OnTime(int sock, short event, void *arg);

	private:
		struct event_base* m_event_base;
	};
}


