#pragma once

#include "IPAddress.hpp"

class IPEndPoint
{
public:
	IPEndPoint() = default;
	IPEndPoint(IPAddress ipAddress, u_short port);
public:
	IPAddress GetAddress() const;
	void SetAddress(IPAddress ipAddress);
	int GetPort() const;
#undef SetPort
	void SetPort(u_short port);
	std::string ToString() const;
public:
	static bool TryParse(std::string_view s, IPEndPoint* ep);
private:
	int _port;
	IPAddress _ipAdr;
};

