#pragma once

#include <functional>
#include <memory>

#include "net/native.hpp"
#include "net/socket.hpp"

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

class context
#ifdef _WIN32 /* WinSock */
    : private OVERLAPPED
#endif
{
    friend class socket;
    friend class IoSystem;

    using callback = std::function<void(context *, bool)>;

  public:
    context();
    ~context();

  public:
    callback completed = [](context *, bool) {};

  public:
    std::unique_ptr<net::socket> acceptSocket;
    std::optional<net::endpoint> endpoint;
    std::span<char> buffer{};
    u_long length = 0;
    void *token;

  private:
    void init();

  private:
    ContextType _contextType;
};
} // namespace net
