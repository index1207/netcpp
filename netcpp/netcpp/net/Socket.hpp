#pragma once

#include "IPEndPoint.hpp"

enum class AddressFamily
{
	Internetwork = AF_INET,
	Internetwork6 = AF_INET6
};

enum class SocketType
{
	Stream = SOCK_STREAM,
	Dgram = SOCK_DGRAM
};

enum class ProtocolType
{
	Tcp = IPPROTO_TCP,
	Udp = IPPROTO_UDP
};

class Socket
{
public:
	Socket() = default;
	Socket(SOCKET s);
	Socket(AddressFamily af, SocketType st, ProtocolType pt);
	~Socket();
public:
	void Connect(IPEndPoint ep);

	void Bind(IPEndPoint ep);
	void Listen(int backlog = SOMAXCONN);
public:
	SOCKET GetHandle() const;
public:
	Socket Accept();
	bool AcceptAsync(class SocketAsyncEventArgs* args);
private:
	SOCKET _sock;
};