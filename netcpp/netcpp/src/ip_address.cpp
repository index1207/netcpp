#include "pch.h"
#include "ip_address.hpp"

using namespace net;

ip_address ip_address::Any = Parse(INADDR_ANY);
ip_address ip_address::None = Parse(INADDR_NONE);
ip_address ip_address::Loopback = Parse("127.0.0.1");
ip_address ip_address::Broadcast = Parse(INADDR_BROADCAST);

ip_address ip_address::Parse(std::string_view ipStr)
{
	ip_address addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	inet_pton(AF_INET, ipStr.data(), &addr.sin_addr);

	return addr;
}

ip_address ip_address::Parse(int ipNum)
{
	ip_address addr{};
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	addr.sin_addr.s_addr = htonl(ipNum);

	return addr;
}

ip_address::ip_address()
{
	ZeroMemory(this, sizeof(SOCKADDR_IN));
	sin_family = AF_INET;
}


ip_address::ip_address(const SOCKADDR_IN& adrs)
{
	sin_addr = adrs.sin_addr;
	sin_family = AF_INET;
	sin_port = adrs.sin_port;
}

std::string ip_address::ToString() const
{
	char ipStrBuf[16] = "";
	inet_ntop(AF_INET, &sin_addr, ipStrBuf, 16);
	return ipStrBuf;
}
