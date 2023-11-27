#pragma once

#include "Socket.hpp"
#include <thread>

namespace net
{
	unsigned CALLBACK worker(HANDLE hcp);

	class IoCore
	{
	public:
		IoCore();
		~IoCore();
	public:
		void push(Socket& sock);
		void push(SOCKET s);
	private:
		HANDLE _hcp;
		std::thread _worker;
	};

	extern IoCore g_dispatcher;
}