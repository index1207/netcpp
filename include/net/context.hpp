#pragma once

#include <functional>
#include <memory>

#include "export.hpp"
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

    class NETCPP_API context
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

        void add_data(char* buffer, int offset, int count);
        void add_data(std::span<char> buffer);
    public:
        std::shared_ptr<net::socket> accept_socket;
        std::optional<net::endpoint> endpoint;
        u_long length;
    private:
        void init();
    private:
        std::span<char> _buffer;
#ifdef _WIN32
        std::vector<WSABUF> _buffer_list;
#else
        std::vector<iovec> _buffer_list;
        // Must outlive the submitted SQE: io_uring may read the msghdr after
        // io_uring_submit() returns (when the request is punted to io-wq).
        msghdr _msg;
#endif

        void* _token;
        io_type _io_type;
    };
}
