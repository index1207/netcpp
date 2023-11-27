#pragma once

#include <string>
#include "IpAddress.hpp"

namespace net
{
	class Endpoint
	{
	public:
		Endpoint() = default;
		Endpoint(IpAddress ipAddress, u_short port);
	public:
		const IpAddress& get_address() const;
		void set_address(IpAddress ipAddress);
		int get_port() const;
		void set_port(u_short port);
		
		std::string to_string() const;
	public:
		static Endpoint parse(SOCKADDR_IN addr);
		static bool try_parse(std::string_view s, Endpoint* ep);
	private:
		int _port;
		IpAddress _ipAdr;
	};
}

