#include <iostream>
#include <mysql/mysql.h>
#include "MyEventTimer.h"
#include "MyEventAcceptor.h"
#include "ServerConntrol.h"

using namespace std;
using namespace GG;

void testMysql()
{
	/*MYSQL conn;
	int res;
	mysql_init(&conn);
	if (mysql_real_connect(&conn, "localhost", "root", "123456", "test", 3306, NULL, CLIENT_FOUND_ROWS));
	{
		cout << "connect success!" << endl;
		res = mysql_query(&conn, "insert into test(Name, Age) values('user',25)");
		if (res)
		{
			cout << "error" << endl;
		}
		else
		{
			cout << "OK" << endl;
		}
		mysql_close(&conn);
	}*/
}


void testEvent()
{
	auto pTimeEvent = new MyEventTimer();
	pTimeEvent->MyEventInit();
	pTimeEvent->start();

	vector<MyEventWorker*> m_works;
	for (int i = 0; i < 2; i++)
	{
		auto pWorker = new MyEventWorker();
		pWorker->SetWorkId(i);
		pWorker->SetEventTimer(pTimeEvent);
		m_works.push_back(pWorker);
	}

	for (auto it : m_works)
	{
		it->MyEventInit();
		it->start();
	}

	auto pAcceptor = new MyEventAcceptor();
	pAcceptor->SetEventWorks(m_works);
	pAcceptor->MyEventInit();
	if (pAcceptor->Joinbale() == false)
	{
		pAcceptor->SetJoinAble(true);
	}
	pAcceptor->start();

	pAcceptor->Join();
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "arg error" << endl;
		return 0;
	}
	if (strcmp(argv[1], "start")  == 0)
	{
		testEvent();
	}
	else
	{
		auto pServerControl = new ServerConntrol();
		pServerControl->Control(argv[1]);
		delete pServerControl;
	}

	return 0;
}
