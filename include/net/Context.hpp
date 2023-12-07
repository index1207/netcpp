#pragma once

#include <memory>
#include <functional>
#include "Socket.hpp"

namespace net
{
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
    public:
		Context();
    public:
		ContextType contextType;
		std::function<void(Context*)> completed = nullptr;
    public:
        std::unique_ptr<Socket> acceptSocket = nullptr;
        void* token = nullptr;
        Endpoint endpoint {};
        std::span<char> buffer {};
        size_t length = 0;
    };
}

