#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#define SOCK_DISCONNECT SD_BOTH

using SOCKLEN = int;
#else
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <cerrno>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <resolv.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <liburing.h>

#include <cstring>

#define SOCKET_ERROR (-1)
#define INVALID_SOCKET SOCKET_ERROR

#define ZeroMemory(addr, size) memset(addr, 0, size)

#define SOCK_DISCONNECT SHUT_RDWR

using SOCKET = int;
using SOCKLEN = socklen_t;

#endif
#include <vector>

#include "net/export.hpp"

namespace net
{
	class context;
	class socket;

	class NETCPP_API native
	{
	public:
	   struct option final
	   {
		   static bool auto_run;
		   static unsigned thread_count;
		   static unsigned long timeout;
		   static u_int entry_count;
	   };

	#ifdef _WIN32
	    static LPFN_ACCEPTEX accept;
	    static LPFN_CONNECTEX connect;
	    static LPFN_DISCONNECTEX disconnect;
	    static LPFN_GETACCEPTEXSOCKADDRS get_accept_socket_address;
	#endif
	public:
	#ifdef _WIN32
	   static HANDLE get_handle();
	#else
	   static io_uring* get_handle();
	#endif
	public:
	    static bool initialize();

		static void run_io(unsigned num);
		static void io(unsigned id);
		static bool observe(socket* sock);
	private:
		static bool demux(context*, u_long, bool);

	private:
	#ifdef _WIN32
		static HANDLE _cp;
	#else
		static std::vector<io_uring*> _io_uring_list;
		static thread_local io_uring* _io_uring;
	#endif
	};
} // namespace net