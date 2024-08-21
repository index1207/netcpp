#include "net/endpoint.hpp"

using namespace net;

endpoint::endpoint(ip_address address, u_short port) : _address(address)
{
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
}

const ip_address& endpoint::get_address() const
{
	return _address;
}

void endpoint::set_address(ip_address address)
{
    _address = address;
}

int endpoint::get_port() const
{
	return ntohs(_address.sin_port);
}

void endpoint::set_port(u_short port)
{
    _address.sin_port = htons(port);
}

std::string endpoint::to_string() const
{
	return _address.to_string() + ":" + std::to_string(get_port());
}

endpoint endpoint::parse(sockaddr_in addr)
{
	endpoint ep;
    ep.set_port(addr.sin_port);
    ep.set_address(ip_address(addr));

	return ep;
}

bool endpoint::try_parse(std::string_view s, endpoint* ep)
{
	auto idx = s.find(':');
	if (idx == std::string::npos)
		return false;

    ip_address addr;
	if (!ip_address::try_parse(s.substr(0, idx), &addr))
        return false;

	auto port = std::stoi(s.substr(idx+1, s.length()).data());
    ep->set_address(addr);
    ep->set_port(port);

	return true;
}

bool endpoint::operator==(const endpoint& endpoint) const {
    return (get_address() == endpoint.get_address()) &&
           (get_port() == endpoint.get_port());
}

bool endpoint::operator==(endpoint&& endpoint) const {
    return (get_address() == endpoint.get_address()) &&
           (get_port() == endpoint.get_port());
}
