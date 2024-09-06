#pragma once

#include <functional>
#include <memory>

#include "net/native.hpp"
#include "net/socket.hpp"

namespace net
{
    enum class io_type
    {
	    none,
	    accept,
	    connect,
	    disconnect,
	    send,
	    receive
    };

    class context
#ifdef _WIN32
        : private OVERLAPPED
#endif
    {
        friend class socket;
        friend class native;

        using callback = std::function<void(context*, bool)>;
    public:
        context();
        ~context();
    public:
        callback completed;
    public:
        void set_buffer(char* buffer, int offset, int count);
        void set_buffer(std::span<char> buffer);
    public:
        std::shared_ptr<net::socket> accept_socket;
        std::optional<net::endpoint> endpoint;
        std::optional<std::vector<std::span<char>>> buffer_list;
        u_long length;
    private:
        void init();
    private:
        std::span<char> _buffer;
        void* _token;
        io_type _io_type;
    };
}
