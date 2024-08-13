#pragma once

#include <memory>
#include <functional>

#include "net/Socket.hpp"

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

#ifdef _WIN32 /* WinSock */
    class Context : private OVERLAPPED
#else /* POSIX */
    class Context
#endif
    {
        friend class Socket;
        friend class Native;

        using Callback = std::function<void(Context*, bool)>;
    public:
		Context();
        ~Context();
    public:
#ifdef _WIN32
        bool createBuffer(DWORD size);
#endif
        Callback completed = [](Context*, bool) {};
    public:
        std::unique_ptr<Socket> acceptSocket;
        std::optional<Endpoint> endpoint;
        char* buffer {};
        u_long length = 0;
        void* token{};
    private:
        void init();
    private:
        ContextType _contextType;
#ifdef _WIN32
        RIO_BUFFERID _bufferId{};
#endif
    };
}

