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
		explicit IpAddress(const sockaddr_in& addr);
	public:
		static bool tryParse(std::string_view ipStr, IpAddress* addr);
		static IpAddress parse(int ipNum);
    public:
        bool operator==(const IpAddress& ipAdr) const;
        bool operator==(IpAddress&& ipAdr) const;
	public:
		[[nodiscard]] std::string toString() const;
    };
}

