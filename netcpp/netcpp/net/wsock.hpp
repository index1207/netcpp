#pragma once

#include <WinSock2.h>
#include <MSWSock.h>

namespace net
{
	class wsock
	{
	public:
		static LPFN_ACCEPTEX AcceptEx;
		static LPFN_CONNECTEX ConnectEx;
		static LPFN_DISCONNECTEX DisconnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
	public:
		static void Initialize();
	};
}