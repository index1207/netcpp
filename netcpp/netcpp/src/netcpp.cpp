#include "pch.h"
#include "netcpp.hpp"

using namespace net;

class netcpp final
{
public:
	netcpp()
	{
		WSADATA wsaData{};
		assert(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

		Extension::Initialize();
	}
	~netcpp()
	{
		WSACleanup();
	}
} _netcpp;