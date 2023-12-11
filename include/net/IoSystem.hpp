#pragma once

#include "Socket.hpp"

#include <vector>
#include <thread>

namespace net
{
	class IoSystem
	{
	public:
		IoSystem();
		~IoSystem();
    public:
        static unsigned CALLBACK worker(HANDLE hcp);
    public:
		void push(Socket& sock);
		void push(SOCKET s);
	private:
		HANDLE _hcp;
        std::vector<std::thread*> _workers;
	};

    extern IoSystem ioSystem;
}