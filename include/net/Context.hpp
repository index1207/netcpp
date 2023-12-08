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

    class Context : private OVERLAPPED
	{
        friend class Socket;
        friend class IoCore;
    public:
		Context();
    public:
		Callback completed = nullptr;
    public:
        std::unique_ptr<Socket> acceptSocket = nullptr;
        std::unique_ptr<Endpoint> endpoint = nullptr;
        std::span<char> buffer {};
        std::atomic<u_long> length = 0;
        std::atomic_bool isSuccess;
    private:
        void init();
    private:
        const Socket* _sock = nullptr;
        ContextType _contextType;
    };
}

