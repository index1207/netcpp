#pragma once

#include <vector>
#include <thread>

#include <concurrent_vector.h>

namespace net
{
	class IoSystem
	{
        friend class Socket;
	public:
		IoSystem();
		~IoSystem();
    public:
		void push(SOCKET s);
    public:
        void worker();
        void dispatch(class Context* context, DWORD numOfBytes, bool isSuccess);
	private:
		HANDLE _hcp;
        const Socket* _listeningSocket;
        concurrency::concurrent_vector<std::thread*> _workers;
	};

    extern IoSystem ioSystem;
}