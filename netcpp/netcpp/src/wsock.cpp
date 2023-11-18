#include "pch.h"
#include "..\net\wsock.hpp"

#include "socket.hpp"

using namespace net;

LPFN_ACCEPTEX wsock::AcceptEx = nullptr;
LPFN_CONNECTEX wsock::ConnectEx = nullptr;
LPFN_DISCONNECTEX wsock::DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS wsock::GetAcceptExSockaddrs = nullptr;

bool BindExtensionFunction(SOCKET s, GUID guid, PVOID* func)
{
	DWORD dwBytes;
	return SOCKET_ERROR != WSAIoctl(s,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(GUID),
		func, sizeof(*func),
		&dwBytes, NULL, NULL
		);
}

void wsock::Initialize()
{
	socket dummy(address_family::IPV4, socket_type::STREAM);
	
	if(!BindExtensionFunction(dummy.get_handle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&AcceptEx)))
		throw std::runtime_error("Can't bind `AcceptEx` function.");
	if(!BindExtensionFunction(dummy.get_handle(), WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&ConnectEx)))
		throw std::runtime_error("Can't bind `ConnectEx` function.");
	if(!BindExtensionFunction(dummy.get_handle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&DisconnectEx)))
		throw std::runtime_error("Can't bind `DisconnectEx` function.");
	if(!BindExtensionFunction(dummy.get_handle(), WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID*>(&wsock::GetAcceptExSockaddrs)))
		throw std::runtime_error("Can't bind `GetAcceptExSockaddrs` function.");
	
	dummy.close();
}
