#pragma once

#include <WinSock2.h>

#include <vector>
#include <thread>
#include <mutex>

namespace net
{
    struct Option final
    {
        static unsigned long long Timeout;
        static unsigned ThreadCount;
    };

	class IoSystem
	{
        friend class Socket;
	private:
		IoSystem();
		~IoSystem();
    public:
        static IoSystem& instance() { static IoSystem ioSystem; return ioSystem; }
    public:
		void push(SOCKET s);
        HANDLE getHandle();
    public:
        DWORD CALLBACK worker();
        void dispatch(class Context* context, DWORD numOfBytes, bool isSuccess);
	private:
		HANDLE _hcp;
        const Socket* _listeningSocket;
        std::mutex mtx;
	};
}