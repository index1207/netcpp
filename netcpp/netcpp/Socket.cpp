#include "pch.h"
#include "net/Socket.hpp"
#include "net/SocketEx.hpp"
#include "net/SocketAsyncEventArgs.hpp"

#include <iostream>

Socket::Socket(AddressFamily af, SocketType st, ProtocolType pt)
{
	_sock = WSASocketW(int(af), int(st), int(pt), NULL, NULL, WSA_FLAG_OVERLAPPED);
	assert(_sock != INVALID_SOCKET);
}

Socket::Socket(SOCKET s)
{
	_sock = s;
}

Socket::~Socket()
{
	closesocket(_sock);
}

void Socket::Connect(IPEndPoint ep)
{
	auto ipAdr = ep.GetAddress();
	assert(SOCKET_ERROR != connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN)));
}

void Socket::Bind(IPEndPoint ep)
{
	auto ipAdr = ep.GetAddress();
	assert(SOCKET_ERROR != bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN)));
}

void Socket::Listen(int backlog)
{
	assert(SOCKET_ERROR != listen(_sock, backlog));
}

SOCKET Socket::GetHandle() const
{
	return _sock;
}

Socket Socket::Accept()
{
	return accept(_sock, nullptr, nullptr);
}

bool Socket::AcceptAsync(class SocketAsyncEventArgs* args)
{
	SOCKET newClient{};
	byte buffer[1 << 6] = { 0, };
	DWORD dwByte = 0;
	if (!SocketEx::AcceptEx(_sock, newClient, buffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, reinterpret_cast<LPOVERLAPPED>(args)
	))
	{
		const auto err = WSAGetLastError();
		args->socketError = (SocketError)err;

		if (err == WSA_IO_PENDING)
			return true;
		return false;
	}
	return false;
}
