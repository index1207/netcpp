#pragma once

namespace net
{
	class Native
	{
	public:
		static LPFN_ACCEPTEX AcceptEx;
		static LPFN_CONNECTEX ConnectEx;
		static LPFN_DISCONNECTEX DisconnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
	public:
		static void initialize();
	};
}