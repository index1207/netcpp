#pragma once

// Socket Utility
#include "Native.hpp"
#include "IpAddress.hpp"
#include "Endpoint.hpp"

// IO
#include "IoCore.hpp"
#include "Socket.hpp"
#include "Context.hpp"

// Network Utility
#include "Dns.hpp"

#include <stdexcept>


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