#include "pch.h"
#include "dns.hpp"
#include "ip_address.hpp"

using namespace net;

std::string dns::GetHostName()
{
	char buf[128] = "";
	gethostname(buf, 128);

	return buf;
}

host_entry dns::GetHostEntry(std::string hostname)
{
	auto entry = gethostbyname(hostname.c_str());
	if (entry == nullptr)
		return host_entry();

	host_entry hostentry { .HostName = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.AddressList.emplace_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.Aliases.emplace_back(entry->h_aliases[i]);
	}

	return hostentry;
}
host_entry dns::GetHostEntry(ip_address ipAddress)
{	
	auto entry = gethostbyaddr(reinterpret_cast<const char*>(&ipAddress.sin_addr), sizeof(IN_ADDR), AF_INET);
	if (entry == nullptr)
		return host_entry();

	host_entry hostentry{ .HostName = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.AddressList.emplace_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.Aliases.emplace_back(entry->h_aliases[i]);
	}

	return hostentry;
}