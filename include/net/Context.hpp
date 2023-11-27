#pragma once

#include <memory>
#include <functional>
#include "Socket.hpp"

namespace net
{
	enum class SocketError
	{
#undef ERROR
		Error = -1,
		Success = 1,
	};

	enum class ContextType
	{
		Accept,
		Connect,
		Disconnect,
		Send,
		Receive
	};

	class Context
		: public OVERLAPPED
	{
		void Init();
	public:
		explicit Context(ContextType eventType);
		ContextType eventType;
		SocketError socketError;
		std::function<void(Context*)> completed;
	};

    class AcceptContext : public Context
    {
    public:
        AcceptContext() : Context(ContextType::Accept), accept_socket(nullptr)
        {
            ZeroMemory(_buf, sizeof(_buf));
        }
        std::unique_ptr<class Socket> accept_socket;
        char _buf[(sizeof(SOCKADDR_IN) + 16) * 2];
    };

    class ConnectContext : public Context
    {
    public:
        ConnectContext() : Context(ContextType::Connect) { }
        Endpoint endpoint {};
    };

    class SendContext : public Context
    {
    public:
        SendContext() : Context(ContextType::Send) { }
        std::span<char> span {};
        int length = 0;
    };

    class ReceiveContext : public Context
    {
    public:
        ReceiveContext() : Context(ContextType::Receive) { }
        std::span<char> span {};
        int length = 0;
    };

    class DisconnectContext : public Context
    {
    public:
        DisconnectContext() : Context(ContextType::Disconnect) { }
    };
}

