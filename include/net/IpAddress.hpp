#pragma once

#include <string>

#include "net/Native.hpp"

namespace net
{
	class IpAddress : private sockaddr_in
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
		explicit IpAddress(const sockaddr_in& adrs);
	public:
		static IpAddress parse(std::string_view ipStr);
		static IpAddress parse(int ipNum);
	public:
		[[nodiscard]] std::string toString() const;
    };
}

