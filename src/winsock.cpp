#include "net/winsock.hpp"
#include "net/Exception.hpp"
#include "net/Native.hpp"

class Winsock final
{
public:
    Winsock()
    {
        WSADATA wsaData{};
        if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw std::runtime_error("Not compatible with this platform.");

        net::Native::initialize();
    }
    ~Winsock()
    {
        WSACleanup();
    }
};

static Winsock _winsock;