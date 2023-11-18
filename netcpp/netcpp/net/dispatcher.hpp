#pragma once

#include "socket.hpp"
#include <thread>

namespace net
{
	unsigned CALLBACK worker(HANDLE hcp);

	class dispatcher
	{
	public:
		dispatcher();
		~dispatcher();
	public:
		void push(socket& sock);
		void push(SOCKET s);
	private:
		HANDLE _hcp;
		std::thread _worker;
	};

	extern dispatcher GNetCore;
}