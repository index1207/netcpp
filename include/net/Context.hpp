#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include "Socket.hpp"

namespace net
{
	enum class ContextType
	{
        None,
		Accept,
		Connect,
		Disconnect,
		Send,
		Receive
	};

    using Callback = std::function<void(Context*)>;

    class Context : public OVERLAPPED
	{
        friend class Socket;
    public:
		Context();
    public:
		ContextType contextType;
		Callback completed = nullptr;
    public:
        std::unique_ptr<Socket> acceptSocket = nullptr;
        Endpoint endpoint {};
        std::span<char> buffer {};
        std::atomic<u_long> length = 0;
    private:
        void init();
        const Socket* _listenSock = nullptr;
    };
}

