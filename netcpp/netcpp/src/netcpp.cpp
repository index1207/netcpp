#include "pch.h"
#include "netcpp.hpp"

using namespace net;

class netcpp final
{
public:
	netcpp()
	{
		WSADATA wsaData{};
		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw std::runtime_error("Not compatible with this platform.");

		wsock::Initialize();
	}
	~netcpp()
	{
		WSACleanup();
	}
} _netcppLib;
