#include "pch.h"
#include "IPEndPoint.hpp"

using namespace net;

IPEndPoint::IPEndPoint(IPAddress ipAddress, u_short port) : _ipAdr(ipAddress), _port(port)
{
	_ipAdr.sin_family = AF_INET;
	_ipAdr.sin_port = htons(_port);
}

IPAddress IPEndPoint::GetAddress() const
{
	return _ipAdr;
}

void IPEndPoint::SetAddress(IPAddress ipAddress)
{
	_ipAdr = ipAddress;
}

int IPEndPoint::GetPort() const
{
	return _port;
}

void IPEndPoint::SetPort(u_short port)
{
	_port = port;
	_ipAdr.sin_port = htons(port);
}

std::string IPEndPoint::ToString() const
{
	return _ipAdr.ToString() + ":" + std::to_string(_port);
}

IPEndPoint IPEndPoint::Parse(SOCKADDR_IN addr)
{
	IPEndPoint ep;
	ep.SetPort(ntohs(addr.sin_port));
	ep.SetAddress(addr);

	return ep;
}

bool IPEndPoint::TryParse(std::string_view s, IPEndPoint* ep)
{
	auto idx = s.find(":");
	if (idx == std::string::npos)
		return false;

	auto ipAdr = IPAddress::Parse(s.substr(0, idx));
	auto port = std::stoi(s.substr(idx+1, s.length()).data());
	ep->SetAddress(ipAdr);
	ep->SetPort(port);

	return true;
}
