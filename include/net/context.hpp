#pragma once

#include <functional>
#include <memory>

#include "net/native.hpp"
#include "net/socket.hpp"

namespace net
{
enum class io_type {
	none,
	accept,
	connect,
	disconnect,
	send,
	receive
};

class context
#ifdef _WIN32 /* WinSock */
    : private OVERLAPPED
#endif
{
    friend class socket;
    friend class native;

    using callback = std::function<void(context *, bool)>;

public:
    context();
    ~context();

public:
    callback completed;

public:
    std::unique_ptr<net::socket> accept_socket;
    std::optional<net::endpoint> endpoint;
    std::span<char> buffer;
    u_long length;

private:
    void init();

private:
	 void* _token;
	 io_type _io_type;
};
} // namespace net
