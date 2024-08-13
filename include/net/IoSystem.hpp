#pragma once

#include <vector>
#include <thread>
#include <mutex>

#include "net/native.hpp"

namespace net
{
#ifdef _WIN32
    struct Option final
    {
        static bool Autorun;
        static unsigned long Timeout;
        static unsigned ThreadCount;
    };
	class IoSystem
	{
        friend class socket;
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
        void dispatch(class context* context, DWORD numOfBytes, bool isSuccess);
	private:
		HANDLE _hcp;
        const socket* _listeningSocket;
	};
#endif
}