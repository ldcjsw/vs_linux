#pragma once
#include <string>

using std::string;

#define SERVER_PORT 9527
struct Client
{
	struct bufferevent *buf_ev;
	int fd;
	unsigned short port;
	char ipAddr[20];
	void* event_work;
};