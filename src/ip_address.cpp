#include "net/ip_address.hpp"

using namespace net;

ip_address ip_address::any = parse(INADDR_ANY);
ip_address ip_address::none = parse(INADDR_NONE);
ip_address ip_address::loopback = parse(INADDR_LOOPBACK);
ip_address ip_address::broadcast = parse(INADDR_BROADCAST);

bool ip_address::try_parse(std::string_view ipStr, ip_address* addr)
{
    if (!addr)
        return false;
	return SOCKET_ERROR != inet_pton(AF_INET, ipStr.data(), &addr->sin_addr);
}

ip_address ip_address::parse(int ipNum)
{
	ip_address addr{};
	ZeroMemory(&addr, sizeof(sockaddr_in));

	addr.sin_addr.s_addr = htonl(ipNum);

	return addr;
}

ip_address::ip_address() : sockaddr_in()
{
	ZeroMemory(this, sizeof(sockaddr_in));
	sin_family = AF_INET;
}


ip_address::ip_address(const sockaddr_in& adrs) : sockaddr_in()
{
	sin_addr = adrs.sin_addr;
	sin_family = AF_INET;
	sin_port = htons(adrs.sin_port);
}

std::string ip_address::to_string() const
{
	char ipStrBuf[16] = "";
	inet_ntop(AF_INET, &sin_addr, ipStrBuf, 16);
	return ipStrBuf;
}

bool ip_address::operator==(const ip_address &ipAdr) const {
    return 0 == memcmp(this, &ipAdr, sizeof(sockaddr_in));
}

bool ip_address::operator==(ip_address &&ipAdr) const {
    return 0 == memcmp(this, &ipAdr, sizeof(sockaddr_in));
}
