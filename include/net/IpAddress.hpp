#pragma once

#include <WinSock2.h>
#include <string>

namespace net
{
	class IpAddress : private SOCKADDR_IN
	{
		friend class Endpoint;
		friend class Dns;
        friend class Socket;
	public:
		static IpAddress Any;
		static IpAddress None;
		static IpAddress Loopback;
		static IpAddress Broadcast;
	public:
		IpAddress();
		explicit IpAddress(const SOCKADDR_IN& adrs);
	public:
		static IpAddress parse(std::string_view ipStr);
		static IpAddress parse(int ipNum);
	public:
		[[nodiscard]] std::string toString() const;
    };
}

