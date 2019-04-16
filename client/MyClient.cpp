#include "MyClient.h"
#include <stdio.h>  
#include <unistd.h>  
#include <strings.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netdb.h>
#include <string.h>
#include <string> 
#include <iostream>

using namespace GG;
using std::cout;
using std::endl;

#define HOST "localhost"  
#define PORT 9527  
#define MAXDATASIZE 1000 

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

void MyClient::clientFun()
{
	int fd, numbytes;
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in server;

	he = gethostbyname(HOST);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *)he->h_addr);

	std::string str = "\n";
	auto pConnect = [&]() {
		int ret = connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
		if (ret != 0)
		{
			int a = 0;
			a++;
		}
	};

	auto pClose = [&]() {
		close(fd);
	};

	auto pSendMsg = [&]() {
		send(fd, str.c_str(), str.size(), 0);
	};

	auto pGetMsg = [&]() {
		numbytes = recv(fd, buf, static_cast<size_t>(MAXDATASIZE), 0);
		buf[numbytes] = '\0';
	};

	auto pParseMsg = [&]() {
		cout << strlen(buf) << endl;
	};

	pConnect();

	for (int i = 0; i < 100; i++)
	{
		str = std::to_string(0) + str;
		pSendMsg();
		pGetMsg();
		pParseMsg();
		sleep(1);
	}

	pClose();
}
