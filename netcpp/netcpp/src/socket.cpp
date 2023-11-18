#include "pch.h"
#include "socket.hpp"

#include <iostream>

#include "wsock.hpp"
#include "io_context.hpp"
#include "dispatcher.hpp"

using namespace net;

socket::socket(address_family af, socket_type st, protocol pt)
{
	_sock = ::socket(static_cast<int>(af), static_cast<int>(st), static_cast<int>(pt));
	if(_sock == INVALID_SOCKET)
		throw std::runtime_error("socket handle was invalid");
}

socket::socket(address_family af, socket_type st) : socket(af, st, static_cast<protocol>(0))
{
}

socket::socket(const socket& sock)
{
	_sock = sock._sock;
	set_local(sock.get_local());
	set_remote(sock.get_remote());
}

socket::socket(socket&& sock) noexcept
{
	_sock = sock._sock;
	set_local(sock.get_local());
	set_remote(sock.get_remote());
}

net::socket::~socket()
{
	close();
}

socket::socket()
{
	_sock = INVALID_SOCKET;
}

void socket::set_handle(SOCKET s)
{
	_sock = s;
}

void socket::close()
{
	if (_sock != INVALID_SOCKET)
	{
		closesocket(_sock);
		_sock = INVALID_SOCKET;
	}
}

bool socket::connect(endpoint ep)
{
	set_remote(ep);
	ip_address ipAdr = _remoteEp.GetAddress();
	return SOCKET_ERROR != ::connect(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
}

bool socket::bind(endpoint ep)
{
	set_local(ep);
	ip_address ipAdr = _localEp.GetAddress();
	const auto& ret = ::bind(_sock, reinterpret_cast<SOCKADDR*>(&ipAdr), sizeof(SOCKADDR_IN));
	GNetCore.push(*this);
	return SOCKET_ERROR != ret;
}

bool socket::listen(int backlog) const
{
	return SOCKET_ERROR != ::listen(_sock, backlog);
}

SOCKET socket::get_handle() const
{
	return _sock;
}

endpoint socket::get_remote() const
{
	return _remoteEp;
}

endpoint socket::get_local() const
{
	return _localEp;
}

void socket::set_remote(endpoint ep)
{
	_remoteEp = ep;
}

void socket::set_local(endpoint ep)
{
	_localEp = ep;
}

void socket::disconnect()
{
	shutdown(_sock, SD_BOTH);
	close();
}

bool net::socket::disconnect_async(context::disconnect* disconnectEvent) const
{
	if (!wsock::DisconnectEx(_sock, disconnectEvent, 0, 0))
	{
		const int err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}

net::socket socket::accept() const
{
	socket clientSock;
	clientSock.set_handle(::accept(_sock, nullptr, nullptr));

	return clientSock;
}	

bool socket::accept_async(context::accept* event) const
{
	if (event != nullptr)
		event->acceptSocket.reset(new socket(address_family::IPV4, socket_type::STREAM));
	else
		event->acceptSocket = std::make_unique<socket>(address_family::IPV4, socket_type::STREAM);
	
	DWORD dwByte = 0;
	ZeroMemory(&event->_acceptexBuffer, (sizeof(SOCKADDR_IN) + 16) * 2);
	if (!wsock::AcceptEx(_sock, event->acceptSocket->get_handle(), event->_acceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&dwByte, event))
	{
		const auto err = WSAGetLastError();
		return err == WSA_IO_PENDING;
	}
	return false;
}		

bool socket::connect_async(context::connect* event)
{
	bind(endpoint(ip_address::Any, 0));
	ip_address ipAdr = event->endPoint.GetAddress();
	DWORD dw;
	if (!wsock::ConnectEx(_sock,
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

int socket::send(ArraySegment seg) const
{
	return ::send(_sock, seg.Array + seg.Offset, seg.Count - seg.Offset, NULL);
}

int socket::send(ArraySegment seg, endpoint target) const
{
	auto& addr = target.GetAddress();
	return sendto(_sock,
		seg.Array + seg.Offset, seg.Count - seg.Offset,
		NULL,
		reinterpret_cast<const sockaddr*>(&addr), sizeof(SOCKADDR_IN)
		);
}

bool socket::send_async(context::send* sendEvent) const
{
	WSABUF wsaBuf;
	wsaBuf.buf = sendEvent->segment.Array + sendEvent->segment.Offset;
	wsaBuf.len = sendEvent->segment.Count;

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

int socket::receive(ArraySegment seg) const
{
	return 0 < recv(_sock, seg.Array + seg.Offset, seg.Count - seg.Offset, NULL);
}

int socket::receive(ArraySegment seg, endpoint target) const
{
	auto& addr = const_cast<ip_address&>(target.GetAddress());
	int len = sizeof(SOCKADDR_IN);
	return 0 < recvfrom(_sock,
		seg.Array + seg.Offset, seg.Count - seg.Offset,
		NULL, reinterpret_cast<sockaddr*>(&addr), &len);
}

bool socket::receive_async(context::receive* recvEvent) const
{
	WSABUF wsaBuf = {
		.len = static_cast<ULONG>(recvEvent->segment.Count),
		.buf = recvEvent->segment.Array + recvEvent->segment.Offset
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

void socket::set_blocking(bool isBlocking) const
{
	u_long opt = !isBlocking;
	ioctlsocket(_sock, FIONBIO, &opt);
}

void socket::set_linger(linger linger) const
{
	set_socket_option(option_level::SOCKET, option_name::LINGER, linger);
}

void socket::set_broadcast(bool isBroadcast) const
{
	BOOL val = isBroadcast;
	set_socket_option(option_level::SOCKET, option_name::BROADCAST, val);
}

void socket::set_reuse_address(bool isReuseAddr) const
{
	BOOL val = isReuseAddr;
	set_socket_option(option_level::SOCKET, option_name::REUSE_ADDRESS, val);
}

void socket::set_no_delay(bool isNoDelay) const
{
	DWORD val = isNoDelay;
	set_socket_option(static_cast<option_level>(protocol::TCP), option_name::NO_DELAY, val);
}

void socket::set_ttl(int ttl) const
{
	set_socket_option(option_level::IP, option_name::TTL, ttl);
}

void socket::set_send_buffer(int size) const
{
	set_socket_option(option_level::SOCKET, option_name::SEND_BUFFER, size);
}

void socket::set_receive_buffer(int size) const
{
	set_socket_option(option_level::SOCKET, option_name::RECV_BUFFER, size);
}

bool socket::valid() const
{
	return INVALID_SOCKET != _sock;
}

net::socket& socket::operator=(const socket& sock)
{
	_sock = sock._sock;
	set_local(sock.get_local());
	set_remote(sock.get_remote());
	return *this;
}

net::socket& socket::operator=(socket&& sock) noexcept
{
	_sock = sock._sock;
	set_local(sock.get_local());
	set_remote(sock.get_remote());
	return *this;
}
