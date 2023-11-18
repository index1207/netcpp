#include "pch.h"
#include "..\net\endpoint.hpp"

using namespace net;

endpoint::endpoint(ip_address ipAddress, u_short port) : _ipAdr(ipAddress), _port(port)
{
	_ipAdr.sin_family = AF_INET;
	_ipAdr.sin_port = htons(_port);
}

const ip_address& endpoint::GetAddress() const
{
	return _ipAdr;
}

void endpoint::SetAddress(ip_address ipAddress)
{
	_ipAdr = ipAddress;
}

int endpoint::GetPort() const
{
	return _port;
}

void endpoint::SetPort(u_short port)
{
	_port = port;
	_ipAdr.sin_port = htons(port);
}

std::string endpoint::ToString() const
{
	return _ipAdr.ToString() + ":" + std::to_string(_port);
}

endpoint endpoint::Parse(SOCKADDR_IN addr)
{
	endpoint ep;
	ep.SetPort(ntohs(addr.sin_port));
	ep.SetAddress(addr);

	return ep;
}

bool endpoint::TryParse(std::string_view s, endpoint* ep)
{
	auto idx = s.find(":");
	if (idx == std::string::npos)
		return false;

	auto ipAdr = ip_address::Parse(s.substr(0, idx));
	auto port = std::stoi(s.substr(idx+1, s.length()).data());
	ep->SetAddress(ipAdr);
	ep->SetPort(port);

	return true;
}
