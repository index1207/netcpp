#include "net/socket.hpp"
#include <net/exception.hpp>

#include "net/context.hpp"
#include "net/native.hpp"

using namespace net;

socket::socket(protocol pt) : socket()
{
    socket::create(pt);
}

socket::socket(const socket &sock)
{
    _sock = sock._sock;
    _local_endpoint = sock._local_endpoint;
    _remote_endpoint = sock._remote_endpoint;
}

socket::socket(socket &&sock) noexcept
{
    _sock = sock._sock;
    std::swap(_local_endpoint, sock._local_endpoint);
    std::swap(_remote_endpoint, sock._remote_endpoint);
}

net::socket::~socket()
{
    close();
}

socket::socket()
{
    _sock = INVALID_SOCKET;
}

void socket::set_handle(SOCKET s)
{
    _sock = s;
}

void socket::close()
{
    if (_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(_sock);
#else
        ::close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
}

bool socket::connect(endpoint ep)
{
    _remote_endpoint = ep;
    ip_address ipAdr = ep.get_address();
    auto r = SOCKET_ERROR != ::connect(_sock, reinterpret_cast<sockaddr *>(&ipAdr), sizeof(sockaddr_in));
    return r;
}

bool socket::bind(endpoint ep)
{
    _local_endpoint = ep;
    ip_address ipAdr = _local_endpoint->get_address();
    const auto ret = ::bind(_sock, reinterpret_cast<sockaddr *>(&ipAdr), sizeof(sockaddr_in));
#ifdef _WIN32
    native::register_to_iocp(get_handle());
#endif
    return SOCKET_ERROR != ret;
}

bool socket::listen(int backlog) const
{
    return SOCKET_ERROR != ::listen(_sock, backlog);
}

SOCKET socket::get_handle() const
{
    return _sock;
}

std::optional<endpoint> socket::get_remote_endpoint() const
{
    return _remote_endpoint;
}

std::optional<endpoint> socket::get_local_endpoint() const
{
    return _local_endpoint;
}

void socket::disconnect()
{
    shutdown(_sock, NET_SOCK_SHUTDOWN);
    _remote_endpoint = std::nullopt;
}

net::socket socket::accept() const
{
    socket clientSock;
    clientSock.set_handle(::accept(_sock, nullptr, nullptr));

    return clientSock;
}

bool socket::send(std::span<char> s) const
{
    return SOCKET_ERROR != ::send(_sock, s.data(), static_cast<int>(s.size()), 0);
}

bool socket::send(std::span<char> s, endpoint target) const
{
    auto &addr = target.get_address();
    return SOCKET_ERROR == sendto(_sock, s.data(), static_cast<int>(s.size()), 0,
                                  reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_in));
}

int socket::receive(std::span<char> s) const
{
    auto ret = recv(_sock, s.data(), static_cast<int>(s.size()), 0);
    return static_cast<int>(ret);
}

int socket::receive(std::span<char> s, endpoint target) const
{
    auto &addr = const_cast<ip_address &>(target.get_address());
    SOCKLEN len = sizeof(sockaddr_in);
    auto ret = recvfrom(_sock, s.data(), static_cast<int>(s.size()), 0, reinterpret_cast<sockaddr *>(&addr), &len);
    return static_cast<int>(ret);
}

bool socket::set_blocking(bool blocking) const
{
#ifdef _WIN32
    u_long opt = !blocking;
    return SOCKET_ERROR != ioctlsocket(_sock, FIONBIO, &opt);
#else
    int flags = fcntl(_sock, F_GETFL, 0);
    if (flags == SOCKET_ERROR)
        return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return 0 == fcntl(_sock, F_SETFL, flags);
#endif
}

bool socket::set_linger(options::linger linger) const
{
    ::linger lingerData{
#ifdef _WIN32
        .l_onoff = static_cast<u_short>(linger.enabled), .l_linger = static_cast<u_short>(linger.time)
#else
        .l_onoff = static_cast<int>(linger.enabled), .l_linger = linger.time
#endif
    };
    return set_option(options::level::socket, option::linger, &lingerData);
}

bool socket::set_broadcast(bool broadcast) const
{
    int value = broadcast;
    return set_option(options::level::socket, option::broadcast, value);
}

bool socket::set_reuse_address(bool reuse) const
{
#ifdef _WIN32
    return set_option(options::level::socket, option::reuse_address, static_cast<BOOL>(reuse));
#else
    return set_option(options::level::socket, option::reuse_address, static_cast<int>(reuse));
#endif
}

bool socket::set_no_delay(bool no_delay) const
{
#ifdef _WIN32
    return set_option(static_cast<options::level>(protocol::tcp), option::no_delay, static_cast<DWORD>(no_delay));
#else
    return set_option(static_cast<options::level>(protocol::tcp), option::no_delay, static_cast<int>(no_delay));
#endif
}

bool socket::set_ttl(int ttl) const
{
    return set_option(options::level::ip, option::ttl, ttl);
}

bool socket::set_send_buffer(int size) const
{
    return set_option(options::level::socket, option::send_buffer, size);
}

bool socket::set_receive_buffer(int size) const
{
    return set_option(options::level::socket, option::receive_buffer, size);
}

bool socket::is_open() const
{
    return INVALID_SOCKET != _sock;
}

net::socket &socket::operator=(socket&& sock) noexcept
{
    this->_sock = sock._sock;
    std::swap(_local_endpoint, sock._local_endpoint);
    std::swap(_remote_endpoint, sock._remote_endpoint);
    return *this;
}

net::socket &socket::operator=(const socket& sock) = default;

void socket::create(protocol pt)
{
    auto type = socket_type::stream;
    if (pt == protocol::udp)
        type = socket_type::dgram;
    _sock = ::socket(PF_INET, static_cast<int>(type), static_cast<int>(pt));
}

bool socket::operator==(const socket& sock) const
{
    return _sock == sock._sock;
}

bool socket::operator==(socket&& sock) const
{
    return _sock == sock._sock;
}
void async_socket::create(protocol protocol)
{
	auto type = socket_type::stream;
	if (protocol == protocol::udp)
		type = socket_type::dgram;
#ifdef _WIN32
	_sock = WSASocketW(AF_INET, static_cast<int>(type), static_cast<int>(protocol), NULL, NULL, WSA_FLAG_REGISTERED_IO);
#else
	socket::create(protocol);
#endif
}
async_socket::async_socket(protocol protocol)
{
	async_socket::create(protocol);
}

bool async_socket::accept(context* context)
{
	context->init();

	context->type = context::io_type::accept;
#ifdef _WIN32
    context->token = this;
	native::register_to_iocp(get_handle());

	DWORD dwByte = 0;
	char buf[(sizeof(SOCKADDR_IN) + 16) * 2] = "";
	if (!native::acceptex(_sock, context->accept_socket->get_handle(), buf, 0, sizeof(SOCKADDR_IN) + 16,
						  sizeof(SOCKADDR_IN) + 16, &dwByte, context))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
#endif
	return false;
}

bool async_socket::connect(context* context)
{
	context->init();
	context->type = context::io_type::connect;
#ifdef _WIN32
	bind(endpoint(ip_address::any, 0));
	_remote_endpoint = _local_endpoint;

	context->token = static_cast<void *>(this);

	ip_address ipAdr = context->endpoint->get_address();
	DWORD dw;
	if (!native::connectex(_sock, reinterpret_cast<SOCKADDR *>(&ipAdr), sizeof(SOCKADDR_IN), nullptr, NULL, &dw,
						   reinterpret_cast<LPOVERLAPPED>(context)))
	{
		const auto err = WSAGetLastError();
		return WSA_IO_PENDING == err;
	}
#endif
	return false;
}

bool async_socket::send(context* context) const
{
	context->init();
	context->type = context::io_type::send;
#ifdef _WIN32
    RIO_BUF buf {
        .BufferId = context->_buffer_id,
        .Offset = 0,
        .Length = sizeof(context->buffer)
    };
    if (!native::rio.RIOSend(_request_queue, &buf, 1, NULL, context))
    {
        const int err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#endif
	return true;
}

bool async_socket::receive(context* context) const
{
	context->init();
	context->type = context::io_type::receive;
#ifdef _WIN32
	RIO_BUF buf {
        .BufferId = context->_buffer_id,
		.Offset = 0,
		.Length = sizeof(context->buffer)
    };
    if (!native::rio.RIOReceive(_request_queue, &buf, 1, NULL, context))
    {
        const int err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#endif
	return true;
}

bool async_socket::disconnect(context* context) const
{
	context->init();

	context->type = context::io_type::disconnect;
#ifdef _WIN32
	if (!native::disconnectex(_sock, reinterpret_cast<LPOVERLAPPED>(context), 0, 0))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
#else

#endif
	return false;
}
async_socket::async_socket() : socket::socket()
{
    _request_queue = RIO_INVALID_RQ;
}
