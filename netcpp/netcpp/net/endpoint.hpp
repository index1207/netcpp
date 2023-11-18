#pragma once

#include <string>
#include "ip_address.hpp"

namespace net
{
	class endpoint
	{
	public:
		endpoint() = default;
		endpoint(ip_address ipAddress, u_short port);
	public:
		const ip_address& GetAddress() const;
		void SetAddress(ip_address ipAddress);
		int GetPort() const;
	#undef SetPort
		void SetPort(u_short port);
		std::string ToString() const;
	public:
		static endpoint Parse(SOCKADDR_IN addr);
		static bool TryParse(std::string_view s, endpoint* ep);
	private:
		int _port;
		ip_address _ipAdr;
	};
}

