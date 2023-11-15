#include "pch.h"
#include "Socket.hpp"

#include <iostream>

#include "Extension.hpp"
#include "SocketAsyncEvent.hpp"
#include "NetCore.hpp"

using namespace net;

Socket::Socket(AddressFamily af, SocketType st, ProtocolType pt)
{
	_sock = socket(static_cast<int>(af), static_cast<int>(st), static_cast<int>(pt));
	if(_sock == INVALID_SOCKET)
		throw std::runtime_error("Socket handle was invalid");
}

Socket::Socket(AddressFamily af, SocketType st) : Socket(af, st, static_cast<ProtocolType>(0))
{
}

Socket::Socket(const Socket& sock)
{
	_sock = sock._sock;
	SetLocalEndPoint(sock.GetLocalEndPoint());
	SetRemoteEndPoint(sock.GetRemoteEndPoint());
}

Socket::Socket(Socket&& sock) noexcept
{
	_sock = sock._sock;
	SetLocalEndPoint(sock.GetLocalEndPoint());
	SetRemoteEndPoint(sock.GetRemoteEndPoint());
}

Socket::Socket()
{
	_sock = INVALID_SOCKET;
}

Socket::~Socket()
{
	Close();
}

void Socket::SetHandle(SOCKET s)
{
	_sock = s;
}

void Socket::Close()
{
	if (_sock != INVALID_SOCKET)
	{
		closesocket(_sock);
		_sock = INVALID_SOCKET;
	}
}

bool Socket::Connect(IPEndPoint ep)
{
	SetRemoteEndPoint(ep);
	IPAddress ipAdr = _remoteEp.GetAddress();
	return SOCKET_ERROR != connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::Bind(IPEndPoint ep)
{
	SetLocalEndPoint(ep);
	IPAddress ipAdr = _localEp.GetAddress();
	const auto& ret = bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
	GNetCore.Register(*this);
	return SOCKET_ERROR != ret;
}

bool Socket::Listen(int backlog) const
{
	return SOCKET_ERROR != listen(_sock, backlog);
}

SOCKET Socket::GetHandle() const
{
	return _sock;
}

IPEndPoint Socket::GetRemoteEndPoint() const
{
	return _remoteEp;
}

IPEndPoint Socket::GetLocalEndPoint() const
{
	return _localEp;
}

void Socket::SetRemoteEndPoint(IPEndPoint ep)
{
	_remoteEp = ep;
}

void Socket::SetLocalEndPoint(IPEndPoint ep)
{
	_localEp = ep;
}

void net::Socket::Disconnect()
{
	shutdown(_sock, SD_BOTH);
	Close();
}

bool net::Socket::DisconnectAsync(const std::shared_ptr<DisconnectEvent>& disconnectEvent) const
{
	if (!Extension::DisconnectEx(_sock, disconnectEvent.get(), 0, 0))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}

Socket Socket::Accept() const
{
	Socket clientSock;
	clientSock.SetHandle(accept(_sock, nullptr, nullptr));

	return clientSock;
}	

bool Socket::AcceptAsync(std::shared_ptr<AcceptEvent> event) const
{
	event->acceptSocket = std::make_shared<Socket>(AddressFamily::Internetwork, SocketType::Stream);
	
	DWORD dwByte = 0;
	ZeroMemory(&event->_acceptexBuffer, (sizeof(SOCKADDR_IN) + 16) * 2);
	if (!Extension::AcceptEx(_sock, event->acceptSocket->GetHandle(), event->_acceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, event.get()))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}		

bool Socket::ConnectAsync(std::shared_ptr<ConnectEvent> event)
{
	Bind(IPEndPoint(IPAddress::Any, 0));
	IPAddress ipAdr = event->endPoint.GetAddress();
	DWORD dw;
	if (!Extension::ConnectEx(_sock,
		reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN),
		nullptr, NULL,
		&dw, event.get())
		)
	{
		const auto err = WSAGetLastError();
		return WSA_IO_PENDING == err;
	}
	return false;
}

int Socket::Send(ArraySegment seg) const
{
	return send(_sock, seg.Array + seg.Offset, seg.Count - seg.Offset, NULL);
}

int Socket::SendTo(ArraySegment seg, IPEndPoint target) const
{
	auto& addr = target.GetAddress();
	return sendto(_sock,
		seg.Array + seg.Offset, seg.Count - seg.Offset,
		NULL,
		reinterpret_cast<const sockaddr*>(&addr), sizeof(SOCKADDR_IN)
		);
}

bool Socket::SendAsync(std::shared_ptr<SendEvent> sendEvent) const
{
	WSABUF wsaBuf;
	wsaBuf.buf = sendEvent->segment.Array + sendEvent->segment.Offset;
	wsaBuf.len = sendEvent->segment.Count;

	DWORD sentBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(_sock,
		&wsaBuf, 1,
		&sentBytes, flags,
		sendEvent.get(), nullptr)
		)
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return true;
}

int Socket::Receive(ArraySegment seg) const
{
	return 0 < recv(_sock, seg.Array + seg.Offset, seg.Count - seg.Offset, NULL);
}

int Socket::ReceiveFrom(ArraySegment seg, IPEndPoint target) const
{
	auto& addr = const_cast<IPAddress&>(target.GetAddress());
	int len = sizeof(SOCKADDR_IN);
	return 0 < recvfrom(_sock,
		seg.Array + seg.Offset, seg.Count - seg.Offset,
		NULL, reinterpret_cast<sockaddr*>(&addr), &len);
}

bool Socket::ReceiveAsync(std::shared_ptr<RecvEvent> recvEvent) const
{
	WSABUF wsaBuf = {
		.len = static_cast<ULONG>(recvEvent->segment.Count),
		.buf = recvEvent->segment.Array + recvEvent->segment.Offset
	};
	
	DWORD recvBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSARecv(_sock,
		&wsaBuf, 1,
		&recvBytes, &flags,
		recvEvent.get(), nullptr)
		)
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return true;
}

void Socket::SetBlocking(bool isBlocking) const
{
	u_long opt = !isBlocking;
	ioctlsocket(_sock, FIONBIO, &opt);
}

void Socket::SetLinger(LingerOption linger) const
{
	SetSocketOption(SocketOptionLevel::Socket, SocketOptionName::Linger, linger);
}

void Socket::SetBroadcast(bool isBroadcast) const
{
	BOOL val = isBroadcast;
	SetSocketOption(SocketOptionLevel::Socket, SocketOptionName::Broadcast, val);
}

void Socket::SetReuseAddress(bool isReuseAddr) const
{
	BOOL val = isReuseAddr;
	SetSocketOption(SocketOptionLevel::Socket, SocketOptionName::ReuseAddress, val);
}

void Socket::SetNoDelay(bool isNoDelay) const
{
	DWORD val = isNoDelay;
	SetSocketOption(static_cast<SocketOptionLevel>(ProtocolType::Tcp), SocketOptionName::NoDelay, val);
}

void Socket::SetTTL(int ttl) const
{
	SetSocketOption(SocketOptionLevel::IP, SocketOptionName::TTL, ttl);
}

void Socket::SetSendBufferSize(int size) const
{
	SetSocketOption(SocketOptionLevel::Socket, SocketOptionName::SendBuffer, size);
}

void Socket::SetReceiveBufferSize(int size) const
{
	SetSocketOption(SocketOptionLevel::Socket, SocketOptionName::ReceiveBuffer, size);
}

bool Socket::IsValid() const
{
	return INVALID_SOCKET != _sock;
}

Socket& Socket::operator=(const Socket& sock)
{
	_sock = sock._sock;
	SetLocalEndPoint(sock.GetLocalEndPoint());
	SetRemoteEndPoint(sock.GetRemoteEndPoint());
	return *this;
}

Socket& Socket::operator=(Socket&& sock) noexcept
{
	_sock = sock._sock;
	SetLocalEndPoint(sock.GetLocalEndPoint());
	SetRemoteEndPoint(sock.GetRemoteEndPoint());
	return *this;
}
