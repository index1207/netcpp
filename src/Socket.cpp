#include "net/Socket.hpp"
#include <net/Exception.hpp>

#include "net/Native.hpp"
#include "net/Context.hpp"
#include "net/IoSystem.hpp"

using namespace net;

Socket::Socket(Protocol pt) : Socket()
{
    create(pt);
}

Socket::Socket(const Socket& sock)
{
    _sock = sock._sock;
    _localEndpoint = sock._localEndpoint;
    _remoteEndpoint = sock._remoteEndpoint;
}

Socket::Socket(Socket&& sock) noexcept
{
    _sock = sock._sock;
    std::swap(_localEndpoint, sock._localEndpoint);
    std::swap(_remoteEndpoint, sock._remoteEndpoint);
}

net::Socket::~Socket()
{
    close();
}

Socket::Socket()
{
	_sock = INVALID_SOCKET;
}

void Socket::setHandle(SOCKET s)
{
	_sock = s;
}

void Socket::close()
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

bool Socket::connect(Endpoint ep)
{
    _remoteEndpoint = ep;
	IpAddress ipAdr = ep.getAddress();
	auto ret =  SOCKET_ERROR != ::connect(_sock, reinterpret_cast<sockaddr*>(&ipAdr), sizeof(sockaddr_in));
    if (ret)
    {
        sockaddr_in remoteAddrIn {};
        SOCKLEN len = sizeof(remoteAddrIn);
        ret &= SOCKET_ERROR != getpeername(_sock, reinterpret_cast<sockaddr*>(&remoteAddrIn), &len);
        _remoteEndpoint = Endpoint::parse(remoteAddrIn);
    }
    return ret;
}

bool Socket::bind(Endpoint ep)
{
    _localEndpoint = ep;
	IpAddress ipAdr = _localEndpoint->getAddress();
    const auto ret = ::bind(_sock, reinterpret_cast<sockaddr*>(&ipAdr), sizeof(sockaddr_in));
#ifdef _WIN32
    IoSystem::instance().push(_sock);
#endif
	return SOCKET_ERROR != ret;
}

bool Socket::listen(int backlog) const
{
#ifdef _WIN32
    IoSystem::instance()._listeningSocket = this;
#endif
	return SOCKET_ERROR != ::listen(_sock, backlog);
}

SOCKET Socket::getHandle() const
{
	return _sock;
}

std::optional<Endpoint> Socket::getRemoteEndpoint() const
{
    return _remoteEndpoint;
}

std::optional<Endpoint> Socket::getLocalEndpoint() const
{
    return _localEndpoint;
}

void Socket::disconnect()
{
	shutdown(_sock, NET_SOCK_SHUTDOWN);
    _remoteEndpoint = std::nullopt;
}

net::Socket Socket::accept() const
{
	Socket clientSock;
    clientSock.setHandle(::accept(_sock, nullptr, nullptr));

	return clientSock;
}	

bool Socket::accept(Context *context) const {
    context->init();

    context->_contextType = ContextType::Accept;
#ifdef _WIN32
    IoSystem::instance().push(context->acceptSocket->getHandle());

    DWORD dwByte = 0;
    char buf[(sizeof(SOCKADDR_IN) + 16) * 2] = "";
    if (!Native::AcceptEx(_sock, context->acceptSocket->getHandle(), buf, 0,
                          sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                          &dwByte, context)) {
        const auto err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#endif
    return false;
}

bool Socket::connect(Context* context)
{
    context->init();
    context->_contextType = ContextType::Connect;
#ifdef _WIN32
    bind(Endpoint(IpAddress::Any, 0));
    _remoteEndpoint = _localEndpoint;

    context->token = static_cast<void*>(this);

	IpAddress ipAdr = context->endpoint->getAddress();
	DWORD dw;
	if (!Native::ConnectEx(_sock,
                           reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN),
                           nullptr, NULL,
                           &dw, reinterpret_cast<LPOVERLAPPED>(context))
		)
	{
		const auto err = WSAGetLastError();
		return WSA_IO_PENDING == err;
	}
#endif
	return false;
}

bool Socket::send(Context* context) const
{
    context->init();
    context->_contextType = ContextType::Send;
#ifdef _WIN32
    WSABUF wsaBuf;
    wsaBuf.buf = context->buffer.data();
    wsaBuf.len = static_cast<ULONG>(context->buffer.size());

    if (SOCKET_ERROR == WSASend(_sock,
                                &wsaBuf, 1,
                                &wsaBuf.len, 0,
                                reinterpret_cast<LPOVERLAPPED>(context), nullptr)
            )
    {
        const int err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#endif
    return true;
}

bool Socket::receive(Context* context) const
{
    context->init();
    context->_contextType = ContextType::Receive;
#ifdef _WIN32
    WSABUF wsaBuf = {
            .len = static_cast<ULONG>(context->buffer.size()),
            .buf = context->buffer.data()
    };

    DWORD recvBytes = 0, flags = 0;
    if (SOCKET_ERROR == WSARecv(_sock,
                                &wsaBuf, 1,
                                &recvBytes, &flags,
                                reinterpret_cast<LPOVERLAPPED>(context), nullptr)
            )
    {
        const int err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#endif
    return true;
}

bool net::Socket::disconnect(Context* context) const
{
    context->init();

    context->_contextType = ContextType::Disconnect;
#ifdef _WIN32
    if (!Native::DisconnectEx(_sock, reinterpret_cast<LPOVERLAPPED>(context), 0, 0))
    {
        const int err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
#else

#endif
    return false;
}

bool Socket::send(std::span<char> s) const
{
	return SOCKET_ERROR != ::send(_sock, s.data(), static_cast<int>(s.size()), 0);
}

bool Socket::send(std::span<char> s, Endpoint target) const
{
	auto& addr = target.getAddress();
	return SOCKET_ERROR == sendto(_sock,
		s.data(),
        static_cast<int>(s.size()),
		0,
		reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)
		);
}

int Socket::receive(std::span<char> s) const
{
	auto ret = recv(_sock, s.data(), static_cast<int>(s.size()), 0);
    return static_cast<int>(ret);
}

int Socket::receive(std::span<char> s, Endpoint target) const
{
	auto& addr = const_cast<IpAddress&>(target.getAddress());
    SOCKLEN len = sizeof(sockaddr_in);
	auto ret = recvfrom(_sock,
		s.data(), static_cast<int>(s.size()),
		0, reinterpret_cast<sockaddr*>(&addr), &len);
    return static_cast<int>(ret);
}

bool Socket::setBlocking(bool isBlocking) const
{
#ifdef _WIN32
    u_long opt = !isBlocking;
	return SOCKET_ERROR != ioctlsocket(_sock, FIONBIO, &opt);
#else
    int flags = fcntl(_sock, F_GETFL, 0);
    if (flags == SOCKET_ERROR)
        return false;
    flags = isBlocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return 0 == fcntl(_sock, F_SETFL, flags);
#endif
}

bool Socket::setLinger(Linger linger) const
{
    ::linger lingerData {
#ifdef _WIN32
        .l_onoff = static_cast<u_short>(linger.enabled),
        .l_linger = static_cast<u_short>(linger.time)
#else
        .l_onoff = static_cast<int>(linger.enabled),
        .l_linger = linger.time
#endif
    };
    return setOption(OptionLevel::Socket, OptionName::Linger, &lingerData);
}

bool Socket::setBroadcast(bool isBroadcast) const
{
    int value = isBroadcast;
    return setOption(OptionLevel::Socket, OptionName::Broadcast, value);
}

bool Socket::setReuseAddress(bool isReuseAddr) const
{
#ifdef _WIN32
    return setOption(OptionLevel::Socket, OptionName::ReuseAddress, static_cast<BOOL>(isReuseAddr));
#else
    return setOption(OptionLevel::Socket, OptionName::ReuseAddress, static_cast<int>(isReuseAddr));
#endif
}

bool Socket::setNoDelay(bool isNoDelay) const
{
#ifdef _WIN32
    return setOption(static_cast<OptionLevel>(Protocol::Tcp), OptionName::NoDelay, static_cast<DWORD>(isNoDelay));
#else
    return setOption(static_cast<OptionLevel>(Protocol::Tcp), OptionName::NoDelay, static_cast<int>(isNoDelay));
#endif
}

bool Socket::setTTL(int ttl) const
{
    return setOption(OptionLevel::IP, OptionName::TTL, ttl);
}

bool Socket::setSendBuffer(int size) const
{
    return setOption(OptionLevel::Socket, OptionName::SendBuffer, size);
}

bool Socket::setReceiveBuffer(int size) const
{
    return setOption(OptionLevel::Socket, OptionName::RecvBuffer, size);
}

bool Socket::isOpen() const
{
	return INVALID_SOCKET != _sock;
}

Socket& Socket::operator=(Socket&& sock) noexcept {
    this->_sock = sock._sock;
    std::swap(_localEndpoint, sock._localEndpoint);
    std::swap(_remoteEndpoint, sock._remoteEndpoint);
    return *this;
}

Socket &Socket::operator=(const Socket& sock) = default;

void Socket::create(Protocol pt) {
    auto type = SocketType::Stream;
    if(pt == Protocol::Udp) type = SocketType::Dgram;
    _sock = socket(PF_INET, static_cast<int>(type), static_cast<int>(pt));
}

bool Socket::operator==(const Socket& sock) const {
    return _sock == sock._sock;
}

bool Socket::operator==(Socket&& sock) const {
    return _sock == sock._sock;
}
