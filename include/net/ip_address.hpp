#pragma once

#include <string>

#include "net/native.hpp"

namespace net
{
	class ip_address : private sockaddr_in
	{
		friend class endpoint;
		friend class dns;
        friend class socket;
	public:
		static ip_address any;
		static ip_address none;
		static ip_address loopback;
		static ip_address broadcast;
	public:
		ip_address();
		explicit ip_address(const sockaddr_in& addr);
	public:
		static bool try_parse(std::string_view ipStr, ip_address* addr);
		static ip_address parse(int ipNum);
    public:
        bool operator==(const ip_address& ipAdr) const;
        bool operator==(ip_address&& ipAdr) const;
	public:
		[[nodiscard]] std::string to_string() const;
    };
}

