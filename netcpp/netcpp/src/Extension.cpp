#include "pch.h"
#include <Extension.hpp>
#include <Socket.hpp>
#include <iostream>

LPFN_ACCEPTEX Extension::AcceptEx = NULL;
LPFN_CONNECTEX Extension::ConnectEx = NULL;
LPFN_DISCONNECTEX Extension::DisconnectEx = NULL;
LPFN_GETACCEPTEXSOCKADDRS Extension::GetAcceptExSockaddrs = NULL;

void Extension::Initialize()
{
	Socket dummy(AddressFamily::Internetwork, SocketType::Stream);
	assert(BindExtensionFunction(dummy.GetHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&AcceptEx)));
	assert(BindExtensionFunction(dummy.GetHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&ConnectEx)));
	assert(BindExtensionFunction(dummy.GetHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&DisconnectEx)));
	assert(BindExtensionFunction(dummy.GetHandle(), WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID*>(&Extension::GetAcceptExSockaddrs)));
}

bool Extension::BindExtensionFunction(SOCKET s, GUID guid, PVOID* func)
{
	DWORD dwBytes;
	return SOCKET_ERROR != WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), func, sizeof(*func), &dwBytes, NULL, NULL);
}
