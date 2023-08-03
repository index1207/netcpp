#include "pch.h"
#include <Socket.hpp>
#include <Extension.hpp>
#include <SocketAsyncEventArgs.hpp>
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

bool Socket::Connect(IPEndPoint ep)
{
	_remoteEp = &ep;
	IPAddress ipAdr = _remoteEp->GetAddress();
	return SOCKET_ERROR != connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::Bind(IPEndPoint ep)
{
	_localEp = &ep;
	IPAddress ipAdr = _localEp->GetAddress();
	return SOCKET_ERROR != bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool Socket::Listen(int backlog)
{
	GNetCore.Register(this);
	return SOCKET_ERROR != listen(_sock, backlog);
}

SOCKET Socket::GetHandle() const
{
	return _sock;
}

IPEndPoint* Socket::GetRemoteEndPoint() const
{
	return _remoteEp;
}

IPEndPoint* Socket::GetLocalEndPoint() const
{
	return _localEp;
}

void Socket::SetRemoteEndPoint(IPEndPoint ep)
{
	_remoteEp = &ep;
}

void Socket::SetLocalEndPoint(IPEndPoint ep)
{
	_localEp = &ep;
}

Socket Socket::Accept()
{
	return accept(_sock, nullptr, nullptr);
}

bool Socket::AcceptAsync(class SocketAsyncEventArgs* args)
{
	args->type = EventType::Accept;
	args->AcceptSocket = std::make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream);

	DWORD dwByte = 0;
	if (!Extension::AcceptEx(_sock, args->AcceptSocket->GetHandle(), &args->AcceptSocket->buffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, reinterpret_cast<LPOVERLAPPED>(args)))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}	
	return false;
}

void Socket::SetBlocking(bool isBlocking)
{
	u_long opt = !isBlocking;
	ioctlsocket(_sock, FIONBIO, &opt);
}
