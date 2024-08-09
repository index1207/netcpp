#include "net/IpAddress.hpp"

using namespace net;

IpAddress IpAddress::Any = parse(INADDR_ANY);
IpAddress IpAddress::None = parse(INADDR_NONE);
IpAddress IpAddress::Loopback = parse(INADDR_LOOPBACK);
IpAddress IpAddress::Broadcast = parse(INADDR_BROADCAST);

IpAddress IpAddress::parse(std::string_view ipStr)
{
	IpAddress addr{};
	ZeroMemory(&addr, sizeof(sockaddr_in));

	inet_pton(AF_INET, ipStr.data(), &addr.sin_addr);

	return addr;
}

IpAddress IpAddress::parse(int ipNum)
{
	IpAddress addr{};
	ZeroMemory(&addr, sizeof(sockaddr_in));

	addr.sin_addr.s_addr = htonl(ipNum);

	return addr;
}

IpAddress::IpAddress() : sockaddr_in()
{
	ZeroMemory(this, sizeof(sockaddr_in));
	sin_family = AF_INET;
}


IpAddress::IpAddress(const sockaddr_in& adrs) : sockaddr_in()
{
	sin_addr = adrs.sin_addr;
	sin_family = AF_INET;
	sin_port = adrs.sin_port;
}

std::string IpAddress::toString() const
{
	char ipStrBuf[16] = "";
	inet_ntop(AF_INET, &sin_addr, ipStrBuf, 16);
	return ipStrBuf;
}

bool IpAddress::operator==(const IpAddress &ipAdr) const {
    return 0 == memcmp(this, &ipAdr, sizeof(sockaddr_in));
}

bool IpAddress::operator==(IpAddress &&ipAdr) const {
    return 0 == memcmp(this, &ipAdr, sizeof(sockaddr_in));
}
