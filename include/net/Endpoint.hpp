#pragma once

#include <string>

#include "net/Native.hpp"
#include "net/IpAddress.hpp"

namespace net
{
	class Endpoint
	{
        friend class IoSystem;
	public:
		Endpoint() = default;
		Endpoint(IpAddress ipAddress, unsigned short port);
	public:
		const IpAddress& getAddress() const;
		void set_address(IpAddress ipAddress);
		int getPort() const;
		void setPort(unsigned short port);
		
		std::string toString() const;
	public:
		static Endpoint parse(sockaddr_in addr);
		static bool tryParse(std::string_view s, Endpoint* ep);
	private:
		int _port;
		IpAddress _ipAdr;
	};
}

