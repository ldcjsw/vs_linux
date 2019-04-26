#include "MyClient.h"
#include <vector>
#include <unistd.h>

using std::vector;
using namespace GG;

int main()
{
	auto pClient = new MyClient();

	pClient->start();

	pClient->join();
	

	delete pClient;

	return 0;
}
