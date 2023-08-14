#pragma once

namespace net
{
	class Extension
	{
	public:
		static LPFN_ACCEPTEX AcceptEx;
		static LPFN_CONNECTEX ConnectEx;
		static LPFN_DISCONNECTEX DisconnectEx;
		static LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
	public:
		static void Initialize();
	private:
		static bool BindExtensionFunction(SOCKET s, GUID guid, PVOID* func);
	};

	template<typename T>
	static inline bool SetSocketOption(SOCKET s, int level, int optname, T& value)
	{
		return SOCKET_ERROR != ::setsockopt(s, level, optname, reinterpret_cast<char*>(&value), sizeof(value));
	}
}