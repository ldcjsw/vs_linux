#include "ServerConntrol.h"
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

#define HOST "localhost"  
#define PORT 8080  
#define MAXDATASIZE 100

ServerConntrol::ServerConntrol()
{
}

ServerConntrol::~ServerConntrol()
{
}

void ServerConntrol::Control(string controlCommand)
{
	int fd;
	struct hostent *he;
	struct sockaddr_in server;

	he = gethostbyname(HOST);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *)he->h_addr);

	int ret = connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if (ret != 0)
	{
		return;
	}

	controlCommand += '\n';

	send(fd, controlCommand.c_str(), controlCommand.size(), 0);

	close(fd);
}

