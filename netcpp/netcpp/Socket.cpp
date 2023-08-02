#include "pch.h"
#include "net/Socket.hpp"
#include "net/SocketEx.hpp"
#include "net/SocketAsyncEventArgs.hpp"

#include <iostream>
#include <chrono>
#include <thread>

Socket::Socket(AddressFamily af, SocketType st, ProtocolType pt)
{
	_sock = WSASocketW(int(af), int(st), int(pt), NULL, NULL, WSA_FLAG_OVERLAPPED);
	assert(_sock != INVALID_SOCKET);
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
	/*if(_sock != INVALID_SOCKET)
		closesocket(_sock);*/
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
	args->type = EventType::Accept;
	args->AcceptSocket = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);

	DWORD dwByte = 0;
	if (!SocketEx::AcceptEx(_sock, args->AcceptSocket.GetHandle(), buffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, reinterpret_cast<LPOVERLAPPED>(args)))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}	
	return false;
}
