#pragma once

#include "Socket.hpp"
#include <thread>

namespace net
{
	unsigned CALLBACK Worker(HANDLE hcp);

	class NetCore
	{
	public:
		NetCore();
		~NetCore();
	public:
		void Register(Socket& sock);
		void Register(SOCKET s);
	private:
		HANDLE _hcp;
		std::thread _worker;
	};

	extern NetCore GNetCore;
}