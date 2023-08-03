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
	Socket();
	Socket(SOCKET s);
	Socket(AddressFamily af, SocketType st, ProtocolType pt);
	Socket(AddressFamily af, SocketType st);
	~Socket();
public:
	bool Connect(IPEndPoint ep);

	bool Bind(IPEndPoint ep);
	bool Listen(int backlog = SOMAXCONN);
public:
	SOCKET GetHandle() const;
	IPEndPoint* GetRemoteEndPoint() const;
	IPEndPoint* GetLocalEndPoint() const;
public:
	void SetRemoteEndPoint(IPEndPoint ep);
	void SetLocalEndPoint(IPEndPoint ep);
public:
	Socket Accept();
	bool AcceptAsync(class SocketAsyncEventArgs* args);
public:
	void SetBlocking(bool isBlocking);
private:
	IPEndPoint* _remoteEp;
	IPEndPoint* _localEp;
	SOCKET _sock;
public:
	char buffer[(sizeof(SOCKADDR_IN)+16)*2] = {0,};
};