#pragma once

#include <string>
#include <vector>

namespace net
{
	struct HostEntry
	{
		std::vector<class IpAddress> addressList;
		std::vector<std::string> aliases;
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

