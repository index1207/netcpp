#pragma once

#include "Socket.hpp"
#include <thread>

namespace net
{
	class IoCore
	{
	public:
		IoCore();
		~IoCore();
    public:
        static unsigned CALLBACK worker(HANDLE hcp);
    public:
		void push(Socket& sock);
		void push(SOCKET s);
	private:
		HANDLE _hcp;
		std::thread _worker;
	};

	extern IoCore ioCore;
}