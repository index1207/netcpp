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

    class Context : private OVERLAPPED
	{
        friend class Socket;
        friend class IoSystem;

        using Callback = std::function<void(Context*, bool)>;
    public:
		Context();
        ~Context();
    public:
        Callback completed;
    public:
        std::unique_ptr<Socket> acceptSocket;
        std::optional<Endpoint> endpoint;
        std::span<char> buffer {};
        std::atomic<u_long> length = 0;
    private:
        void init();
    private:
        ContextType _contextType;
    };
}

