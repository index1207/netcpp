#pragma once

#include <string>
#include <vector>

namespace net
{
	struct IPHostEntry
	{
		std::vector<class IPAddress> AddressList;
		std::vector<std::string> Aliases;
		std::string HostName;
	};

	class Dns
	{
	public:
		static std::string GetHostName();
		static IPHostEntry GetHostEntry(std::string hostname);
		static IPHostEntry GetHostEntry(IPAddress ipAddress);
	};
}

