#include "net/Endpoint.hpp"

using namespace net;

Endpoint::Endpoint(IpAddress ipAddress, u_short port) : _ipAdr(ipAddress)
{
	_ipAdr.sin_family = AF_INET;
	_ipAdr.sin_port = htons(port);
}

const IpAddress& Endpoint::getAddress() const
{
	return _ipAdr;
}

void Endpoint::setAddress(IpAddress ipAddress)
{
	_ipAdr = ipAddress;
}

int Endpoint::getPort() const
{
	return ntohs(_ipAdr.sin_port);
}

void Endpoint::setPort(u_short port)
{
	_ipAdr.sin_port = htons(port);
}

std::string Endpoint::toString() const
{
	return _ipAdr.toString() + ":" + std::to_string(getPort());
}

Endpoint Endpoint::parse(sockaddr_in addr)
{
	Endpoint ep;
    ep.setPort(htons(addr.sin_port));
	ep.setAddress(IpAddress(addr));

	return ep;
}

bool Endpoint::tryParse(std::string_view s, Endpoint* ep)
{
	auto idx = s.find(':');
	if (idx == std::string::npos)
		return false;

    IpAddress addr;
	if (!IpAddress::tryParse(s.substr(0, idx), &addr))
        return false;

	auto port = std::stoi(s.substr(idx+1, s.length()).data());
    ep->setAddress(addr);
    ep->setPort(port);

	return true;
}

bool Endpoint::operator==(const Endpoint& endpoint) const {
    return (getAddress() == endpoint.getAddress()) &&
           (getPort() == endpoint.getPort());
}

bool Endpoint::operator==(Endpoint&& endpoint) const {
    return (getAddress() == endpoint.getAddress()) &&
           (getPort() == endpoint.getPort());
}
