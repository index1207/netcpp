#pragma once

#include <WinSock2.h>

#include <vector>
#include <thread>
#include <mutex>

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
        static DWORD CALLBACK worker(HANDLE cp);
        static void dispatch(class Context* context, DWORD numOfBytes, bool isSuccess);
	private:
		HANDLE _hcp;
        const static Socket* _listeningSocket;
        std::vector<std::thread*> _workers;
        std::mutex mtx;
	};

    extern IoSystem ioSystem;
}