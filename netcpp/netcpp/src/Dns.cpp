#include "pch.h"
#include "Dns.hpp"
#include "IPAddress.hpp"

using namespace net;

std::string net::Dns::GetHostName()
{
	char buf[128] = "";
	gethostname(buf, 128);

	return buf;
}

net::IPHostEntry net::Dns::GetHostEntry(std::string hostname)
{
	auto entry = gethostbyname(hostname.c_str());
	if (entry == nullptr)
		return IPHostEntry();

	IPHostEntry hostentry { .HostName = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.AddressList.push_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.Aliases.push_back(entry->h_aliases[i]);
	}

	return hostentry;
}
IPHostEntry Dns::GetHostEntry(IPAddress ipAddress)
{	
	auto entry = gethostbyaddr(reinterpret_cast<const char*>(&ipAddress.sin_addr), sizeof(IN_ADDR), AF_INET);
	if (entry == nullptr)
		return IPHostEntry();

	IPHostEntry hostentry{ .HostName = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		SOCKADDR_IN addr_in;
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostentry.AddressList.push_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostentry.Aliases.push_back(entry->h_aliases[i]);
	}

	return hostentry;
}