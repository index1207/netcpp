#pragma once

#include <string>
#include "IpAddress.hpp"

namespace net
{
	class Endpoint
	{
        friend class IoSystem;
	public:
		Endpoint() = default;
		Endpoint(IpAddress ipAddress, u_short port);
	public:
		const IpAddress& getAddress() const;
		void set_address(IpAddress ipAddress);
		int getPort() const;
		void setPort(u_short port);
		
		std::string toString() const;
	public:
		static Endpoint parse(SOCKADDR_IN addr);
		static bool tryParse(std::string_view s, Endpoint* ep);
	private:
		int _port;
		IpAddress _ipAdr;
	};
}

