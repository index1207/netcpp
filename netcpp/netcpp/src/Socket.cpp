#include "pch.h"
#include "Socket.hpp"

#include <iostream>

#include "Extension.hpp"
#include "SocketAsyncEvent.hpp"
#include "NetCore.hpp"

using namespace net;

Socket::Socket(AddressFamily af, SocketType st, ProtocolType pt)
{
	_sock = socket(int(af), int(st), int(pt));
	assert(_sock != INVALID_SOCKET);
}

Socket::Socket(AddressFamily af, SocketType st) : Socket(af, st, (ProtocolType)0)
{
}

Socket::Socket(const Socket& sock)
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
	GNetCore.Register(this);
	return SOCKET_ERROR != ret;
}

bool Socket::Listen(int backlog)
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

bool net::Socket::DisconnectAsync(std::shared_ptr<DisconnectEvent> disconnectEvent)
{
	if (!Extension::DisconnectEx(_sock, disconnectEvent.get(), 0, 0))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}

Socket Socket::Accept()
{
	Socket clientSock;
	clientSock.SetHandle(accept(_sock, nullptr, nullptr));

	return clientSock;
}

bool Socket::AcceptAsync(std::shared_ptr<AcceptEvent> event)
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
	if (!Extension::ConnectEx(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN), NULL, NULL, &dw, event.get()))
	{
		const auto err = WSAGetLastError();
		return WSA_IO_PENDING == err;
	}
	return false;
}

int Socket::Send(ArraySegment seg)
{
	return send(_sock, seg.Array + seg.Offset, seg.Count, NULL);
}

bool Socket::SendAsync(std::shared_ptr<SendEvent> args)
{
	WSABUF wsaBuf;
	wsaBuf.buf = args->segment.Array + args->segment.Offset;
	wsaBuf.len = args->segment.Count;

	DWORD sentBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(_sock, &wsaBuf, 1, &sentBytes, flags, args.get(), NULL))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return true;
}

int Socket::Receive(ArraySegment seg)
{
	return 0 < recv(_sock, seg.Array + seg.Offset, seg.Count, NULL);
}

bool Socket::ReceiveAsync(std::shared_ptr<RecvEvent> args)
{
	WSABUF wsaBuf;
	wsaBuf.buf = args->segment.Array + args->segment.Offset;
	wsaBuf.len = args->segment.Count;

	DWORD recvBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSARecv(_sock, &wsaBuf, 1, &recvBytes, &flags, args.get(), NULL))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return true;
}

void Socket::SetBlocking(bool isBlocking)
{
	u_long opt = !isBlocking;
	ioctlsocket(_sock, FIONBIO, &opt);
}

bool Socket::IsValid() const
{
	return INVALID_SOCKET != _sock;
}

void net::Socket::operator=(Socket sock)
{
	_sock = sock.GetHandle();
	_remoteEp = std::make_unique<IPEndPoint>(sock.GetRemoteEndPoint());
	_localEp = std::make_unique<IPEndPoint>(sock.GetLocalEndPoint());
}
