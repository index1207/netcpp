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
		[[nodiscard]] const IpAddress& getAddress() const;
		void set_address(IpAddress ipAddress);
		[[nodiscard]] int getPort() const;
		void setPort(unsigned short port);
		
		[[nodiscard]] std::string toString() const;
	public:
		static Endpoint parse(sockaddr_in addr);
		static bool tryParse(std::string_view s, Endpoint* ep);
    public:
        bool operator==(const Endpoint& endpoint) const;
        bool operator==(Endpoint&& endpoint) const;
	private:
		int _port;
		IpAddress _ipAdr;
	};
}

