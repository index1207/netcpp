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
	_sock = WSASocket(int(af), int(st), int(pt), NULL, NULL, WSA_FLAG_OVERLAPPED);
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

IPEndPoint* Socket::GetRemoteEndPoint() const
{
	return _remoteEp.get();
}

IPEndPoint* Socket::GetLocalEndPoint() const
{
	return _localEp.get();
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

bool Socket::AcceptAsync(AcceptEvent* args)
{
	args->AcceptSocket = std::make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream);

	DWORD dwByte = 0;
	ZeroMemory(&args->AcceptSocket->_AcceptexBuffer, (sizeof(SOCKADDR_IN) + 16) * 2);
	if (!Extension::AcceptEx(_sock, args->AcceptSocket->GetHandle(), args->AcceptSocket->_AcceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, reinterpret_cast<LPOVERLAPPED>(args)))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}	
	return false;
}

bool Socket::ConnectAsync(ConnectEvent* args)
{
	Bind(IPEndPoint(IPAddress::Any, 0));
	IPAddress ipAdr = args->EndPoint.GetAddress();
	DWORD dw;
	if (!Extension::ConnectEx(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN), NULL, NULL, &dw, reinterpret_cast<LPOVERLAPPED>(args)))
	{
		const auto err = WSAGetLastError();
		return WSA_IO_PENDING == err;
	}
	return false;
}

int Socket::Send(ArraySegment seg)
{
	return send(_sock, (const char*)(seg.Array + seg.Offset), seg.Count, NULL);
}

bool Socket::SendAsync(SendEvent* args)
{
	WSABUF wsaBuf;
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
