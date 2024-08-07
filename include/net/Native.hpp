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

namespace net
{
	class Native
	{
	public:
#ifdef _WIN32
        static LPFN_ACCEPTEX AcceptEx;
		static LPFN_CONNECTEX ConnectEx;
		static LPFN_DISCONNECTEX DisconnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
#endif
	public:
		static void initialize();
	};
}