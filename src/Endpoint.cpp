#include "PCH.h"
#include "..\net\Endpoint.hpp"

using namespace net;

Endpoint::Endpoint(IpAddress ipAddress, u_short port) : _ipAdr(ipAddress), _port(port)
{
	_ipAdr.sin_family = AF_INET;
	_ipAdr.sin_port = htons(_port);
}

const IpAddress& Endpoint::getAddress() const
{
	return _ipAdr;
}

void Endpoint::set_address(IpAddress ipAddress)
{
	_ipAdr = ipAddress;
}

int Endpoint::getPort() const
{
	return _port;
}

void Endpoint::setPort(u_short port)
{
	_port = port;
	_ipAdr.sin_port = htons(port);
}

std::string Endpoint::toString() const
{
	return _ipAdr.toString() + ":" + std::to_string(_port);
}

Endpoint Endpoint::parse(SOCKADDR_IN addr)
{
	Endpoint ep;
    ep.setPort(ntohs(addr.sin_port));
	ep.set_address(IpAddress(addr));

	return ep;
}

bool Endpoint::tryParse(std::string_view s, Endpoint* ep)
{
	auto idx = s.find(":");
	if (idx == std::string::npos)
		return false;

	auto ipAdr = IpAddress::parse(s.substr(0, idx));
	auto port = std::stoi(s.substr(idx+1, s.length()).data());
	ep->set_address(ipAdr);
    ep->setPort(port);

	return true;
}
