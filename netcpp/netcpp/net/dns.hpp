#pragma once

#include <string>
#include <vector>

namespace net
{
	struct host_entry
	{
		std::vector<class ip_address> AddressList;
		std::vector<std::string> Aliases;
		std::string HostName;
	};

	class dns
	{
	public:
		static std::string GetHostName();
		static host_entry GetHostEntry(std::string hostname);
		static host_entry GetHostEntry(ip_address ipAddress);
	};
}

