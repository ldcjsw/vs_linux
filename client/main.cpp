#include "MyClient.h"
#include <vector>
#include <unistd.h>

using std::vector;
using namespace GG;

int main()
{
	vector<MyClient*> clientArray;
	for (auto i = 0; i < 3; i++)
	{
		auto pClient = new MyClient();
		clientArray.push_back(pClient);
	}

	for (auto it : clientArray)
	{
		it->start();
	}

	sleep(5);

	for (size_t i = 0; i < clientArray.size(); i++)
	{
		delete clientArray[i];
	}

	return 0;
}
