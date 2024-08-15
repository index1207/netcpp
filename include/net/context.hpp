#pragma once

#include <functional>
#include <memory>

#include "net/native.hpp"
#include "net/socket.hpp"

namespace net
{

class context
#ifdef _WIN32 /* WinSock */
    : OVERLAPPED, RIO_BUF
#endif
{
    friend class async_socket;
	friend class native;

    using callback = std::function<void(context *, bool)>;

    enum class io_type
    {
      none,
      accept,
      connect,
      disconnect,
      send,
      receive
    };
  public:
    context();
    ~context();

    void set_data(std::string_view data);

  public:
    callback completed = [](context *, bool) {};

    bool create_buffer(u_long size);

  public:
    std::unique_ptr<net::async_socket> accept_socket;
    std::optional<net::endpoint> endpoint;
    char* buffer;
    u_long length = 0;
    void* token;
	io_type type;

  private:
    void init();
};
} // namespace net
