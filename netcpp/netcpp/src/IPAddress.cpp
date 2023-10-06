#include "pch.h"
#include "IPAddress.hpp"

using namespace net;

IPAddress IPAddress::Any = Parse(INADDR_ANY);
IPAddress IPAddress::None = Parse(INADDR_NONE);
IPAddress IPAddress::Loopback = Parse("127.0.0.1");
IPAddress IPAddress::Broadcast = Parse(INADDR_BROADCAST);

IPAddress IPAddress::Parse(std::string_view ipStr)
{
	IPAddress addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	inet_pton(AF_INET, ipStr.data(), &addr.sin_addr);

	return addr;
}

IPAddress IPAddress::Parse(int ipNum)
{
	IPAddress addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	addr.sin_addr.s_addr = htonl(ipNum);

	return addr;
}

IPAddress::IPAddress()
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	sin_family = AF_INET;
}


IPAddress::IPAddress(const SOCKADDR_IN& adrs)
{
	sin_addr = adrs.sin_addr;
	sin_family = AF_INET;
	sin_port = adrs.sin_port;
}

std::string IPAddress::ToString() const
{
	char ipStrBuf[16] = "";
	inet_ntop(AF_INET, &sin_addr, ipStrBuf, 16);
	return ipStrBuf;
}
