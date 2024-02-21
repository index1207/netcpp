#include "PCH.h"
#include "Dns.hpp"
#include "IpAddress.hpp"

using namespace net;

std::string Dns::getHostName()
{
	char buf[128] = "";
	gethostname(buf, 128);

	return buf;
}

HostEntry Dns::getHostEntry(std::string hostname)
{
	auto entry = gethostbyname(hostname.c_str());
	if (entry == nullptr)
		return HostEntry();

	HostEntry hostentry { .host_name = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.address_list.emplace_back(addr_in);
	}

	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.alias_list.emplace_back(entry->h_aliases[i]);
	}

	return hostentry;
}
HostEntry Dns::getHostEntry(IpAddress ipAddress)
{	
	auto entry = gethostbyaddr(reinterpret_cast<const char*>(&ipAddress.sin_addr), sizeof(IN_ADDR), AF_INET);
	if (entry == nullptr)
		return HostEntry();

	HostEntry hostentry{ .host_name = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.address_list.emplace_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.alias_list.emplace_back(entry->h_aliases[i]);
	}

	return hostentry;
}