#include "PCH.h"
#include "Socket.hpp"

#include <iostream>

#include "Native.hpp"
#include "Context.hpp"
#include "IoCore.hpp"

using namespace net;

Socket::Socket(AddressFamily af, SocketType st, Protocol pt)
{
	_sock = ::socket(static_cast<int>(af), static_cast<int>(st), static_cast<int>(pt));
}

Socket::Socket(AddressFamily af, SocketType st) : Socket(af, st, static_cast<Protocol>(0))
{
}

Socket::Socket(const Socket& sock)
{
	_sock = sock._sock;
    setLocalEndpoint(sock.getLocalEndpoint());
    setRemoteEndpoint(sock.getRemoteEndpoint());
}

Socket::Socket(Socket&& sock) noexcept
{
	_sock = sock._sock;
    setLocalEndpoint(sock.getLocalEndpoint());
    setRemoteEndpoint(sock.getRemoteEndpoint());
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
    setRemoteEndpoint(ep);
	IpAddress ipAdr = _remoteEp.get_address();
	return SOCKET_ERROR != ::connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::bind(Endpoint ep)
{
    setLocalEndpoint(ep);
	IpAddress ipAdr = _localEp.get_address();
	const auto& ret = ::bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
	g_dispatcher.push(*this);
	return SOCKET_ERROR != ret;
}

bool Socket::listen(int backlog) const
{
	return SOCKET_ERROR != ::listen(_sock, backlog);
}

SOCKET Socket::getHandle() const
{
	return _sock;
}

Endpoint Socket::getRemoteEndpoint() const
{
	return _remoteEp;
}

Endpoint Socket::getLocalEndpoint() const
{
	return _localEp;
}

void Socket::setRemoteEndpoint(Endpoint ep)
{
	_remoteEp = ep;
}

void Socket::setLocalEndpoint(Endpoint ep)
{
	_localEp = ep;
}

void Socket::disconnect()
{
	shutdown(_sock, SD_BOTH);
    close();
}

bool net::Socket::disconnect(DisconnectContext* disconnectEvent) const
{
	if (!Native::DisconnectEx(_sock, reinterpret_cast<LPOVERLAPPED>(disconnectEvent), 0, 0))
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

bool Socket::accept(AcceptContext* event) const
{
	if (event != nullptr)
		event->accept_socket.reset(new Socket(AddressFamily::IPv4, SocketType::Stream));
	else
		event->accept_socket = std::make_unique<Socket>(AddressFamily::IPv4, SocketType::Stream);
	
	DWORD dwByte = 0;
	ZeroMemory(&event->_buf, (sizeof(SOCKADDR_IN) + 16) * 2);
	if (!Native::AcceptEx(_sock, event->accept_socket->getHandle(), event->_buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                          &dwByte, event))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}		

bool Socket::connect(ConnectContext* event)
{
	bind(Endpoint(IpAddress::Any, 0));
	IpAddress ipAdr = event->endpoint.get_address();
	DWORD dw;
	if (!Native::ConnectEx(_sock,
                           reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN),
                           nullptr, NULL,
                           &dw, event)
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
	auto& addr = target.get_address();
	return sendto(_sock,
		s.data(),
        static_cast<int>(s.size()),
		NULL,
		reinterpret_cast<const sockaddr*>(&addr), sizeof(SOCKADDR_IN)
		);
}

bool Socket::send(SendContext* sendEvent) const
{
	WSABUF wsaBuf;
	wsaBuf.buf = sendEvent->span.data();
	wsaBuf.len = static_cast<int>(sendEvent->span.size());

	DWORD sentBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(_sock,
		&wsaBuf, 1,
		&sentBytes, flags,
		sendEvent, nullptr)
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
	auto& addr = const_cast<IpAddress&>(target.get_address());
	int len = sizeof(SOCKADDR_IN);
	return recvfrom(_sock,
		s.data(), static_cast<int>(s.size()),
		NULL, reinterpret_cast<sockaddr*>(&addr), &len);
}

bool Socket::receive(ReceiveContext* recvEvent) const
{
	WSABUF wsaBuf = {
		.len = static_cast<ULONG>(recvEvent->span.size()),
		.buf = recvEvent->span.data()
	};
	
	DWORD recvBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSARecv(_sock,
		&wsaBuf, 1,
		&recvBytes, &flags,
		recvEvent, nullptr)
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

net::Socket& Socket::operator=(const Socket& sock)
{
	_sock = sock._sock;
    setLocalEndpoint(sock.getLocalEndpoint());
    setRemoteEndpoint(sock.getRemoteEndpoint());
	return *this;
}

net::Socket& Socket::operator=(Socket&& sock) noexcept
{
	_sock = sock._sock;
    setLocalEndpoint(sock.getLocalEndpoint());
    setRemoteEndpoint(sock.getRemoteEndpoint());
	return *this;
}
