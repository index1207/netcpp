#include "PCH.h"
#include "..\net\Endpoint.hpp"

using namespace net;

Endpoint::Endpoint(IpAddress ipAddress, u_short port) : _ipAdr(ipAddress), _port(port)
{
	_ipAdr.sin_family = AF_INET;
	_ipAdr.sin_port = htons(_port);
}

const IpAddress& Endpoint::get_address() const
{
	return _ipAdr;
}

void Endpoint::set_address(IpAddress ipAddress)
{
	_ipAdr = ipAddress;
}

int Endpoint::get_port() const
{
	return _port;
}

void Endpoint::set_port(u_short port)
{
	_port = port;
	_ipAdr.sin_port = htons(port);
}

std::string Endpoint::to_string() const
{
	return _ipAdr.ToString() + ":" + std::to_string(_port);
}

Endpoint Endpoint::parse(SOCKADDR_IN addr)
{
	Endpoint ep;
	ep.set_port(ntohs(addr.sin_port));
	ep.set_address(IpAddress(addr));

	return ep;
}

bool Endpoint::try_parse(std::string_view s, Endpoint* ep)
{
	auto idx = s.find(":");
	if (idx == std::string::npos)
		return false;

	auto ipAdr = IpAddress::Parse(s.substr(0, idx));
	auto port = std::stoi(s.substr(idx+1, s.length()).data());
	ep->set_address(ipAdr);
	ep->set_port(port);

	return true;
}
