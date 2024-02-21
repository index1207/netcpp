#pragma once

#include <string>
#include <vector>

namespace net
{
	struct HostEntry
	{
		std::vector<class IpAddress> address_list;
		std::vector<std::string> alias_list;
		std::string host_name;
	};

	class Dns
	{
	public:
		static std::string getHostName();
		static HostEntry getHostEntry(std::string hostname);
		static HostEntry getHostEntry(IpAddress host);
	};
}

