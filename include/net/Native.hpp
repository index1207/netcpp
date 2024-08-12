#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#define NET_SOCK_SHUTDOWN SD_BOTH

using SOCKLEN = int;
#else
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>
#include <unistd.h>

#include <cstring>

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET SOCKET_ERROR

#define ZeroMemory(addr, size) memset(addr, 0, size)

#define NET_SOCK_SHUTDOWN SHUT_RDWR

using SOCKET = int;
using SOCKLEN = socklen_t;

#endif

#include <functional>


namespace net
{
    class Context;

	class Native
	{
#ifdef _WIN32
    public:
        // Worker options
        struct Option final
        {
            static bool Autorun;
            static unsigned long Timeout;
            static unsigned ThreadCount;
        };

        // IOCP Extension
        static LPFN_ACCEPTEX acceptEx;
		static LPFN_CONNECTEX connectEx;
		static LPFN_DISCONNECTEX disconnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockAddr;

        // RIO Extension
        static RIO_EXTENSION_FUNCTION_TABLE rioTable;
        static thread_local RIO_CQ completionQue;

        static std::function<void(bool)> onExitIo;
    public:
        static bool addToCompletionPort(SOCKET sock);
#endif
	public:
		static bool initialize();
#ifdef _WIN32
    private:
        static bool handleEvent(Context* context, DWORD bytes, bool success);
        static void ioWorker();
    private:
        static HANDLE _hcp;
#endif
	};
}