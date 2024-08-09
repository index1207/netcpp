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

bool Native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    Socket dummy(Protocol::Tcp);
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID *>(&AcceptEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID *>(&ConnectEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID *>(&DisconnectEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_GETACCEPTEXSOCKADDRS,
                              reinterpret_cast<PVOID *>(&Native::GetAcceptExSockaddrs)))
        return false;
#else
#endif
    return true;
}
