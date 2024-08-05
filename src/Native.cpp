#include "net/Native.hpp"
#include "net/Socket.hpp"

#include <stdexcept>

using namespace net;

#ifdef _WIN32
LPFN_ACCEPTEX Native::AcceptEx = nullptr;
LPFN_CONNECTEX Native::ConnectEx = nullptr;
LPFN_DISCONNECTEX Native::DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS Native::GetAcceptExSockaddrs = nullptr;

bool bindExtensionFunction(SOCKET s, GUID guid, PVOID* func)
{
	DWORD dwBytes;
	return SOCKET_ERROR != WSAIoctl(s,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(GUID),
		func, sizeof(*func),
		&dwBytes, NULL, NULL);
}
#endif

void Native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("Not compatible with this platform.");

    Socket dummy(Protocol::Tcp);
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID *>(&AcceptEx)))
		throw std::runtime_error("Can't bind `AcceptEx` function.");
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID *>(&ConnectEx)))
		throw std::runtime_error("Can't bind `ConnectEx` function.");
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID *>(&DisconnectEx)))
		throw std::runtime_error("Can't bind `DisconnectEx` function.");
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_GETACCEPTEXSOCKADDRS,
                              reinterpret_cast<PVOID *>(&Native::GetAcceptExSockaddrs)))
		throw std::runtime_error("Can't bind `GetAcceptExSockaddrs` function.");
#endif
}
