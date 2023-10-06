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
	_localEp = nullptr;
	_remoteEp = nullptr;
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
	IPAddress ipAdr = _remoteEp->GetAddress();
	return SOCKET_ERROR != connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::Bind(IPEndPoint ep)
{
	SetLocalEndPoint(ep);
	IPAddress ipAdr = _localEp->GetAddress();
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
	return *_remoteEp;
}

IPEndPoint Socket::GetLocalEndPoint() const
{
	return *_localEp;
}

void Socket::SetRemoteEndPoint(IPEndPoint ep)
{
	_remoteEp = std::make_unique<IPEndPoint>(ep);
}

void Socket::SetLocalEndPoint(IPEndPoint ep)
{
	_localEp = std::make_unique<IPEndPoint>(ep);
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

bool Socket::AcceptAsync(const std::shared_ptr<AcceptEvent>& event) const
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

bool Socket::ConnectAsync(const std::shared_ptr<ConnectEvent>& event)
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
	return send(_sock, seg.Array + seg.Offset, seg.Count, NULL);
}

bool Socket::SendAsync(const std::shared_ptr<SendEvent>& sendEvent) const
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
	return 0 < recv(_sock, seg.Array + seg.Offset, seg.Count, NULL);
}

bool Socket::ReceiveAsync(const std::shared_ptr<RecvEvent>& recvEvent) const
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

bool Socket::IsValid() const
{
	return INVALID_SOCKET != _sock;
}

Socket& Socket::operator=(const Socket& sock)
{
	_sock = sock.GetHandle();
	_remoteEp = std::make_unique<IPEndPoint>(sock.GetRemoteEndPoint());
	_localEp = std::make_unique<IPEndPoint>(sock.GetLocalEndPoint());
	return *this;
}

Socket& Socket::operator=(Socket&& sock) noexcept
{
	_sock = sock.GetHandle();
	_remoteEp = std::make_unique<IPEndPoint>(sock.GetRemoteEndPoint());
	_localEp = std::make_unique<IPEndPoint>(sock.GetLocalEndPoint());
	return *this;
}
