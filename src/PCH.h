#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#include <string>
#include <vector>
#include <format>

#include <stdexcept>

#include <net/Exception.hpp>
#include "Native.hpp"


#ifdef _WIN32

class netcpp final
{
public:
    netcpp()
    {
        WSADATA wsaData{};
        if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw std::runtime_error("Not compatible with this platform.");

        net::Native::initialize();
    }
    ~netcpp()
    {
        WSACleanup();
    }
};

static netcpp _netcpp;

#endif