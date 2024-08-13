#pragma once

#include <optional>
#include <span>

#include "net/endpoint.hpp"
#include "net/ip_address.hpp"
#include "net/native.hpp"

namespace net
{
enum class protocol
{
    ip = IPPROTO_IP,
    tcp = IPPROTO_TCP,
    udp = IPPROTO_UDP
};

enum class address_family
{
    ipv4 = AF_INET,
    ipv6 = AF_INET6
};

enum class socket_type
{
    stream = SOCK_STREAM,
    dgram = SOCK_DGRAM
};

enum option
{
    // socket Level
    linger = SO_LINGER,
    reuse_address = SO_REUSEADDR,
    send_buffer = SO_SNDBUF,
    receive_buffer = SO_RCVBUF,
    broadcast = SO_BROADCAST,

#ifdef _WIN32
    accept_context = SO_UPDATE_ACCEPT_CONTEXT,
    connect_context = SO_UPDATE_CONNECT_CONTEXT,
#endif
    // ip Level
    ttl = IP_TTL,
    multicast_ttl = IP_MULTICAST_TTL,

    // tcp Level
    no_delay = TCP_NODELAY
};

namespace options
{
enum class level
{
    ip = IPPROTO_IP,
    ipv6 = IPPROTO_IPV6,
    socket = SOL_SOCKET,
};

struct linger
{
    bool enabled;
    int time;
};
} // namespace options
} // namespace net

namespace net
{
class context;

class socket
{
  public:
    socket();
    explicit socket(protocol pt);
    socket(const socket &sock);
    socket(socket &&sock) noexcept;
    ~socket();

  public:
    void close();
    void create(protocol pt = protocol::ip);

    void set_handle(SOCKET s);

    bool bind(endpoint ep);
    bool listen(int backlog = SOMAXCONN) const;

  public:
    [[nodiscard]] SOCKET get_handle() const;
    [[nodiscard]] std::optional<endpoint> get_remote_endpoint() const;
    [[nodiscard]] std::optional<endpoint> get_local_endpoint() const;

  public:
    void disconnect();
    [[nodiscard]] socket accept() const;
    bool connect(endpoint ep);

    bool send(std::span<char> s) const;
    bool send(std::span<char> s, endpoint target) const;

    int receive(std::span<char> s) const;
    int receive(std::span<char> s, endpoint target) const;

  public:
    bool disconnect(context *context) const;
    bool accept(context *context) const;
    bool connect(context *context);
    bool send(context *context) const;
    bool receive(context *context) const;

  public:
    template <class T> bool set_option(options::level level, option name, T value) const
    {
        if (_sock == INVALID_SOCKET)
            return false;
        return SOCKET_ERROR != setsockopt(_sock, static_cast<int>(level), static_cast<int>(name),
                                          reinterpret_cast<const char *>(&value), sizeof(T));
    }
    template <class T> bool get_option(options::level level, option name, T &value) const
    {
        if (_sock == INVALID_SOCKET)
            return false;
        SOCKLEN optLen = sizeof(T);
        return SOCKET_ERROR != getsockopt(_sock, static_cast<int>(level), static_cast<int>(name),
                                          reinterpret_cast<char *>(&value), &optLen);
    }

    [[nodiscard]] bool set_blocking(bool blocking) const;
    [[nodiscard]] bool set_linger(options::linger linger) const;
    [[nodiscard]] bool set_broadcast(bool broadcast) const;
    [[nodiscard]] bool set_reuse_address(bool reuse) const;
    [[nodiscard]] bool set_no_delay(bool no_delay) const;
    [[nodiscard]] bool set_ttl(int ttl) const;
    [[nodiscard]] bool set_send_buffer(int size) const;
    [[nodiscard]] bool set_receive_buffer(int size) const;

    [[nodiscard]] bool is_open() const;

  public:
    bool operator==(const socket &sock) const;
    bool operator==(socket &&sock) const;

    socket &operator=(const socket &sock);
    socket &operator=(socket &&sock) noexcept;

  private:
    std::optional<endpoint> _remote_endpoint;
    std::optional<endpoint> _local_endpoint;
    SOCKET _sock;
};
} // namespace net