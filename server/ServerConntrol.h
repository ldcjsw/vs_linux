#pragma once

#include <string>

using std::string;

class ServerConntrol
{
public:
	ServerConntrol();
	~ServerConntrol();

	void Control(string controlCommand);
};

