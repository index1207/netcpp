#pragma once

#include <WinSock2.h>
#include <string>

namespace net
{
	class IpAddress : private SOCKADDR_IN
	{
		friend class Endpoint;
		friend class Dns;
	public:
		static IpAddress Any;
		static IpAddress None;
		static IpAddress Loopback;
		static IpAddress Broadcast;
	public:
		IpAddress();
		explicit IpAddress(const SOCKADDR_IN& adrs);
	public:
		static IpAddress Parse(std::string_view ipStr);
		static IpAddress Parse(int ipNum);
	public:
		[[nodiscard]] std::string ToString() const;
	};
}

