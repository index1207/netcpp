#include "PCH.h"
#include "IpAddress.hpp"

using namespace net;

IpAddress IpAddress::Any = Parse(INADDR_ANY);
IpAddress IpAddress::None = Parse(INADDR_NONE);
IpAddress IpAddress::Loopback = Parse("127.0.0.1");
IpAddress IpAddress::Broadcast = Parse(INADDR_BROADCAST);

IpAddress IpAddress::Parse(std::string_view ipStr)
{
	IpAddress addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	inet_pton(AF_INET, ipStr.data(), &addr.sin_addr);

	return addr;
}

IpAddress IpAddress::Parse(int ipNum)
{
	IpAddress addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	addr.sin_addr.s_addr = htonl(ipNum);

	return addr;
}

IpAddress::IpAddress()
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	sin_family = AF_INET;
}


IpAddress::IpAddress(const SOCKADDR_IN& adrs)
{
	sin_addr = adrs.sin_addr;
	sin_family = AF_INET;
	sin_port = adrs.sin_port;
}

std::string IpAddress::ToString() const
{
	char ipStrBuf[16] = "";
	inet_ntop(AF_INET, &sin_addr, ipStrBuf, 16);
	return ipStrBuf;
}
