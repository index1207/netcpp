#include "net/dns.hpp"
#include "net/ip_address.hpp"

using namespace net;

std::string dns::get_host_name()
{
	char buf[128] = "";
	gethostname(buf, 128);
	return buf;
}

host_entry dns::get_host_entry(std::string_view host)
{
	auto entry = gethostbyname(host.data());
	if (entry == nullptr)
		return {};

	host_entry hostEntry { .host_name = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		sockaddr_in addr_in{};
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostEntry.address_list.emplace_back(addr_in);
	}

	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostEntry.alias_list.emplace_back(entry->h_aliases[i]);
	}

	return hostEntry;
}
host_entry dns::get_host_entry(net::ip_address host)
{
	auto entry = gethostbyaddr(reinterpret_cast<const char*>(&host.sin_addr), sizeof(in_addr), AF_INET);
	if (entry == nullptr)
		return {};

	host_entry hostEntry { .host_name = entry->h_name };
	for (int i = 0; entry->h_addr_list[i] != nullptr; ++i)
	{
		sockaddr_in addr_in{};
		addr_in.sin_addr.s_addr = *reinterpret_cast<long*>(entry->h_addr_list[i]);
		hostEntry.address_list.emplace_back(addr_in);
	}
	for (int i = 0; entry->h_aliases[i] != nullptr; ++i)
	{
		hostEntry.alias_list.emplace_back(entry->h_aliases[i]);
	}

	return hostEntry;
}