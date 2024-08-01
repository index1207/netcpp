#include "net/netcpp.hpp"
#include "net/Native.hpp"

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