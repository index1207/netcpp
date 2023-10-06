#pragma once

#include <WinSock2.h>

namespace net
{
	class IPAddress : private SOCKADDR_IN
	{
		friend class IPEndPoint;
		friend class Dns;
	public:
		static IPAddress Any;
		static IPAddress None;
		static IPAddress Loopback;
		static IPAddress Broadcast;
	public:
		IPAddress();
		IPAddress(const SOCKADDR_IN& adrs);
	public:
		static IPAddress Parse(std::string_view ipStr);
		static IPAddress Parse(int ipNum);
	public:
		std::string ToString() const;
	};
}

