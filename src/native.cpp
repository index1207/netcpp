#include "net/native.hpp"
#include "net/socket.hpp"

#include <stdexcept>

using namespace net;

#ifdef _WIN32
LPFN_ACCEPTEX native::AcceptEx = nullptr;
LPFN_CONNECTEX native::ConnectEx = nullptr;
LPFN_DISCONNECTEX native::DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS native::GetAcceptExSockaddrs = nullptr;

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

bool native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    socket dummy(protocol::tcp);
	if(!bindExtensionFunction(dummy.get_handle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID *>(&AcceptEx)))
        return false;
	if(!bindExtensionFunction(dummy.get_handle(), WSAID_CONNECTEX, reinterpret_cast<PVOID *>(&ConnectEx)))
        return false;
	if(!bindExtensionFunction(dummy.get_handle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID *>(&DisconnectEx)))
        return false;
	if(!bindExtensionFunction(dummy.get_handle(), WSAID_GETACCEPTEXSOCKADDRS,
                              reinterpret_cast<PVOID *>(&native::GetAcceptExSockaddrs)))
        return false;
#else
#endif
    return true;
}
