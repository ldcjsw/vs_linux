#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::unordered_map;

#define SERVER_PORT 9527
struct Client
{
	struct bufferevent *buf_ev;
	int fd;
	unsigned short port;
	char ipAddr[20];
	void* event_work;
};