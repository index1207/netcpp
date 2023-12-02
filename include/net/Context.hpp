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
		: private OVERLAPPED
	{
        friend class Socket;
    public:
		Context();
    public:
		ContextType contextType;
		SocketError socketError;
		std::function<void(Context*)> completed;
    public:
        std::unique_ptr<Socket> acceptSocket;
        void* token;
        Endpoint endpoint {};
        std::span<char> buffer {};
        size_t length = 0;
    };
}

