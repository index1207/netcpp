#include "pch.h"
#include <Extension.hpp>
#include <Socket.hpp>

LPFN_ACCEPTEX Extension::AcceptEx = NULL;
LPFN_CONNECTEX Extension::ConnectEx = NULL;
LPFN_DISCONNECTEX Extension::DisconnectEx = NULL;

void Extension::Initialize()
{
	auto dummy = std::make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	assert(BindExtensionFunction(dummy->GetHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&AcceptEx)));
	assert(BindExtensionFunction(dummy->GetHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&ConnectEx)));
	assert(BindExtensionFunction(dummy->GetHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&DisconnectEx)));
	dummy.release();
}

bool Extension::BindExtensionFunction(SOCKET s, GUID guid, PVOID* func)
{
	DWORD dwBytes;
	return SOCKET_ERROR != WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), func, sizeof(*func), &dwBytes, NULL, NULL);
}
