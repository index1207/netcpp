#include "PCH.h"
#include "Socket.hpp"

#include <iostream>

#include "Native.hpp"	
#include "Context.hpp"
#include "IoSystem.hpp"

using namespace net;

Socket::Socket(Protocol pt) : Socket()
{
    create(pt);
}

Socket::Socket(const Socket& sock)
{
	_sock = sock._sock;
    if(sock._localEndpoint != nullptr)
        setLocalEndpoint(*sock._localEndpoint);
    if(sock._remoteEndpoint != nullptr)
        setRemoteEndpoint(*sock._remoteEndpoint);
}

Socket::Socket(Socket&& sock) noexcept
{
	_sock = sock._sock;
    if(sock._localEndpoint != nullptr)
        setLocalEndpoint(*sock._localEndpoint);
    if(sock._remoteEndpoint != nullptr)
        setRemoteEndpoint(*sock._remoteEndpoint);
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
		closesocket(_sock);
		_sock = INVALID_SOCKET;
	}
}

bool Socket::connect(Endpoint ep)
{
    setLocalEndpoint(ep);
	IpAddress ipAdr = _localEndpoint->getAddress();
	return SOCKET_ERROR != ::connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::bind(Endpoint ep)
{
    setLocalEndpoint(ep);
	IpAddress ipAdr = _localEndpoint->getAddress();
    const auto ret = ::bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN6));
	ioSystem.push(_sock);
	return SOCKET_ERROR != ret;
}

bool Socket::listen(int backlog) const
{
    ioSystem._listeningSocket = this;
	return SOCKET_ERROR != ::listen(_sock, backlog);
}

SOCKET Socket::getHandle() const
{
	return _sock;
}

Endpoint Socket::getRemoteEndpoint() const
{
    return *_remoteEndpoint;
}

Endpoint Socket::getLocalEndpoint() const
{
    return *_localEndpoint;
}

void Socket::setRemoteEndpoint(Endpoint ep)
{
	_remoteEndpoint.reset(new Endpoint(ep));
}

void Socket::setLocalEndpoint(Endpoint ep)
{
    _localEndpoint.reset(new Endpoint(ep));
}

void Socket::disconnect()
{
	shutdown(_sock, SD_BOTH);
    close();
}

bool net::Socket::disconnect(Context* context) const
{
    context->init();

    context->_contextType = ContextType::Disconnect;
	if (!Native::DisconnectEx(_sock, reinterpret_cast<LPOVERLAPPED>(context), 0, 0))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
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
    ioSystem.push(context->acceptSocket->_sock);

    DWORD dwByte = 0;
    char buf[(sizeof(SOCKADDR_IN) + 16) * 2] = "";
    if (!Native::AcceptEx(_sock, context->acceptSocket->_sock, buf, 0,
                          sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                          &dwByte, context)) {
        const auto err = WSAGetLastError();
        return err == WSA_IO_PENDING;
    }
    return false;
}

bool Socket::connect(Context* context)
{
    context->init();
    context->_contextType = ContextType::Connect;

    bind(Endpoint(IpAddress::Any, 0));
    _remoteEndpoint = std::move(_localEndpoint);
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
	return false;
}

int Socket::send(std::span<char> s) const
{
	return ::send(_sock, s.data(), static_cast<int>(s.size()), NULL);
}

int Socket::send(std::span<char> s, Endpoint target) const
{
	auto& addr = target.getAddress();
	return sendto(_sock,
		s.data(),
        static_cast<int>(s.size()),
		NULL,
		reinterpret_cast<const sockaddr*>(&addr), sizeof(SOCKADDR_IN)
		);
}

bool Socket::send(Context* context) const
{
    context->init();
    context->_contextType = ContextType::Send;

    WSABUF wsaBuf;
	wsaBuf.buf = context->buffer.data();
	wsaBuf.len = static_cast<int>(context->buffer.size());

	DWORD sentBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(_sock,
		&wsaBuf, 1,
		&sentBytes, flags,
        reinterpret_cast<LPOVERLAPPED>(context), nullptr)
		)
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return true;
}

int Socket::receive(std::span<char> s) const
{
	return recv(_sock, s.data(), static_cast<int>(s.size()), NULL);
}

int Socket::receive(std::span<char> s, Endpoint target) const
{
	auto& addr = const_cast<IpAddress&>(target.getAddress());
	int len = sizeof(SOCKADDR_IN);
	return recvfrom(_sock,
		s.data(), static_cast<int>(s.size()),
		NULL, reinterpret_cast<sockaddr*>(&addr), &len);
}

bool Socket::receive(Context* context) const
{
    context->init();
    context->_contextType = ContextType::Receive;

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
	return true;
}

void Socket::setBlocking(bool isBlocking) const
{
	u_long opt = !isBlocking;
	ioctlsocket(_sock, FIONBIO, &opt);
}

void Socket::setLinger(Linger linger) const
{
    setSocketOption(OptionLevel::Socket, OptionName::Linger, linger);
}

void Socket::setBroadcast(bool isBroadcast) const
{
	BOOL val = isBroadcast;
    setSocketOption(OptionLevel::Socket, OptionName::Broadcast, val);
}

void Socket::setReuseAddress(bool isReuseAddr) const
{
	BOOL val = isReuseAddr;
    setSocketOption(OptionLevel::Socket, OptionName::ReuseAddress, val);
}

void Socket::setNoDelay(bool isNoDelay) const
{
	DWORD val = isNoDelay;
    setSocketOption(static_cast<OptionLevel>(Protocol::Tcp), OptionName::NoDelay, val);
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
    if(sock._remoteEndpoint != nullptr)
	    this->_remoteEndpoint.reset(new Endpoint(*sock._remoteEndpoint));
    if(sock._localEndpoint != nullptr)
	    this->_localEndpoint.reset(new Endpoint(*sock._localEndpoint));

    return *this;
}

Socket &Socket::operator=(const Socket& sock) {
    this->_sock = sock._sock;
    if(sock._remoteEndpoint != nullptr)
        this->_remoteEndpoint.reset(new Endpoint(*sock._remoteEndpoint));
    if(sock._localEndpoint != nullptr)
        this->_localEndpoint.reset(new Endpoint(*sock._localEndpoint));

    return *this;
}

void Socket::create(Protocol pt) {
    auto type = SocketType::Stream;
    if(pt == Protocol::Udp) type = SocketType::Dgram;
    _sock = ::socket(PF_INET, static_cast<int>(type), static_cast<int>(pt));
}

void Socket::BindEndpoint()
{
    SOCKADDR_IN addr;
    int namelen = sizeof(SOCKADDR_IN);
    if(SOCKET_ERROR == getsockname(_sock, reinterpret_cast<SOCKADDR*>(&addr), &namelen))
    {
        throw network_error("getsockname()");
    }
}
