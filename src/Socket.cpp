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
    setLocalEndpoint(ep);
	IpAddress ipAdr = _localEndpoint->getAddress();
	return SOCKET_ERROR != ::connect(_sock, reinterpret_cast<sockaddr*>(&ipAdr), sizeof(sockaddr_in));
}

bool Socket::bind(Endpoint ep)
{
    setLocalEndpoint(ep);
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

void Socket::setRemoteEndpoint(Endpoint ep)
{
	_remoteEndpoint = ep;
}

void Socket::setLocalEndpoint(Endpoint ep)
{
    _localEndpoint = ep;
}

void Socket::disconnect() const
{
	shutdown(_sock, NET_SOCK_SHUTDOWN);
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
    wsaBuf.len = context->buffer.size();

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
	return recv(_sock, s.data(), static_cast<int>(s.size()), 0);
}

int Socket::receive(std::span<char> s, Endpoint target) const
{
	auto& addr = const_cast<IpAddress&>(target.getAddress());
    SOCKLEN len = sizeof(sockaddr_in);
	return recvfrom(_sock,
		s.data(), static_cast<int>(s.size()),
		0, reinterpret_cast<sockaddr*>(&addr), &len);
}

void Socket::setBlocking(bool isBlocking) const
{
	u_long opt = !isBlocking;
#ifdef _WIN32
	ioctlsocket(_sock, FIONBIO, &opt);
#else
    ioctl(_sock, F_SETFL, opt | O_NONBLOCK);
#endif
}

void Socket::setLinger(Linger linger) const
{
    setSocketOption(OptionLevel::Socket, OptionName::Linger, linger);
}

void Socket::setBroadcast(bool isBroadcast) const
{
    setSocketOption(OptionLevel::Socket, OptionName::Broadcast, isBroadcast);
}

void Socket::setReuseAddress(bool isReuseAddr) const
{
#ifdef _WIN32
    setSocketOption(OptionLevel::Socket, OptionName::ReuseAddress, static_cast<BOOL>(isReuseAddr));
#else
    setSocketOption(OptionLevel::Socket, OptionName::ReuseAddress, static_cast<int>(isReuseAddr));
#endif
}

void Socket::setNoDelay(bool isNoDelay) const
{
#ifdef _WIN32
    setSocketOption(static_cast<OptionLevel>(Protocol::Tcp), OptionName::NoDelay, static_cast<DWORD>(isNoDelay));
#else
    setSocketOption(static_cast<OptionLevel>(Protocol::Tcp), OptionName::NoDelay, static_cast<int>(isNoDelay));
#endif
}

void Socket::setTTL(int ttl) const
{
    setSocketOption(OptionLevel::IP, OptionName::TTL, ttl);
}

void Socket::setSendBuffer(int size) const
{
    setSocketOption(OptionLevel::Socket, OptionName::SendBuffer, size);
}

void Socket::setReceiveBuffer(int size) const
{
    setSocketOption(OptionLevel::Socket, OptionName::RecvBuffer, size);
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

Socket &Socket::operator=(const Socket& sock) {
    this->_sock = sock._sock;
    this->_localEndpoint = sock._localEndpoint;
    this->_remoteEndpoint = sock._remoteEndpoint;
   
    return *this;
}

void Socket::create(Protocol pt) {
    auto type = SocketType::Stream;
    if(pt == Protocol::Udp) type = SocketType::Dgram;
    _sock = socket(PF_INET, static_cast<int>(type), static_cast<int>(pt));
}

void Socket::BindEndpoint() const
{
    sockaddr_in addr = {0,};
    SOCKLEN nameLen = sizeof(sockaddr_in);
    if(SOCKET_ERROR == getsockname(_sock, reinterpret_cast<sockaddr*>(&addr), &nameLen))
    {
        throw network_error("getsockname()");
    }
}
