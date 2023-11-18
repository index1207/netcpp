#pragma once

#include <WinSock2.h>

namespace net
{
	class ip_address : private SOCKADDR_IN
	{
		friend class endpoint;
		friend class dns;
	public:
		static ip_address Any;
		static ip_address None;
		static ip_address Loopback;
		static ip_address Broadcast;
	public:
		ip_address();
		ip_address(const SOCKADDR_IN& adrs);
	public:
		static ip_address Parse(std::string_view ipStr);
		static ip_address Parse(int ipNum);
	public:
		std::string ToString() const;
	};
}

