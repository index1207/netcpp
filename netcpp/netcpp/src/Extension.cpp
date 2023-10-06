#include "pch.h"
#include "Extension.hpp"

#include "Socket.hpp"

using namespace net;

LPFN_ACCEPTEX Extension::AcceptEx = nullptr;
LPFN_CONNECTEX Extension::ConnectEx = nullptr;
LPFN_DISCONNECTEX Extension::DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS Extension::GetAcceptExSockaddrs = nullptr;

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

void Extension::Initialize()
{
	Socket dummy(AddressFamily::Internetwork, SocketType::Stream);
	
	if(!BindExtensionFunction(dummy.GetHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&AcceptEx)))
		throw std::runtime_error("Can't bind `AcceptEx` function.");
	if(!BindExtensionFunction(dummy.GetHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&ConnectEx)))
		throw std::runtime_error("Can't bind `ConnectEx` function.");
	if(!BindExtensionFunction(dummy.GetHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&DisconnectEx)))
		throw std::runtime_error("Can't bind `DisconnectEx` function.");
	if(!BindExtensionFunction(dummy.GetHandle(), WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID*>(&Extension::GetAcceptExSockaddrs)))
		throw std::runtime_error("Can't bind `GetAcceptExSockaddrs` function.");
	
	dummy.Close();
}
