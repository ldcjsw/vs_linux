#pragma once
#include "MyThread.h"

namespace GG {
	class MyClient : public MyThread
	{
	public:
		MyClient();
		~MyClient();

		virtual void run();

	private:
		void clientFun();
	};
}


