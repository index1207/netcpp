#include "pch.h"
#include <Socket.hpp>
#include <Extension.hpp>
#include <SocketAsyncEvent.hpp>
#include <NetCore.hpp>

#include <iostream>
#include <chrono>
#include <thread>

Socket::Socket(AddressFamily af, SocketType st, ProtocolType pt)
{
	_sock = socket(int(af), int(st), int(pt));
	assert(_sock != INVALID_SOCKET);
}

Socket::Socket(AddressFamily af, SocketType st) : Socket(af, st, (ProtocolType)0)
{
}

Socket::Socket()
{

}

Socket::Socket(SOCKET s)
{
	_sock = s;
}

Socket::~Socket()
{
	if(_sock != INVALID_SOCKET)
		closesocket(_sock);
}

void Socket::Close()
{
	closesocket(_sock);
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

Socket Socket::Accept()
{
	return accept(_sock, nullptr, nullptr);
}

bool Socket::AcceptAsync(AcceptEvent* event)
{
	event->AcceptSocket = new Socket(AddressFamily::Internetwork, SocketType::Stream);

	DWORD dwByte = 0;
	ZeroMemory(&event->AcceptSocket->_AcceptexBuffer, (sizeof(SOCKADDR_IN) + 16) * 2);
	if (!Extension::AcceptEx(_sock, event->AcceptSocket->GetHandle(), event->AcceptSocket->_AcceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, event))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}	
	return false;
}

bool Socket::ConnectAsync(ConnectEvent* event)
{
	Bind(IPEndPoint(IPAddress::Any, 0));
	IPAddress ipAdr = event->EndPoint.GetAddress();
	DWORD dw;
	if (!Extension::ConnectEx(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN), NULL, NULL, &dw, event))
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

bool Socket::SendAsync(SendEvent* args)
{
	WSABUF wsaBuf;
	wsaBuf.buf = args->segment.Array + args->segment.Offset;
	wsaBuf.len = args->segment.Count;

	DWORD sentBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(_sock, &wsaBuf, 1, &sentBytes, flags, args, NULL))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}

int Socket::Receive(ArraySegment seg)
{
	return 0 < recv(_sock, seg.Array + seg.Offset, seg.Count, NULL);
}

bool Socket::ReceiveAsync(RecvEvent* args)
{
	WSABUF wsaBuf;
	wsaBuf.buf = args->segment.Array + args->segment.Offset;
	wsaBuf.len = args->segment.Count;

	DWORD recvBytes = 0, flags = 0;
	if (SOCKET_ERROR == WSARecv(_sock, &wsaBuf, 1, &recvBytes, &flags, args, NULL))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
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
