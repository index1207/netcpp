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

struct ArraySegment
{
	byte* Array;
	int Offset;
	int Count;
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
	void Close();

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
	bool AcceptAsync(class AcceptEvent* args);

	bool Connect(IPEndPoint ep);
	bool ConnectAsync(class ConnectEvent* args);

	int Send(ArraySegment seg);
	bool SendAsync(class SendEvent* args);

	int Receive(ArraySegment seg);
	bool ReceiveAsync(class ReceiveEvent* args);
public:
	void SetBlocking(bool isBlocking);
	bool IsValid() const;
private:
	std::unique_ptr<IPEndPoint> _remoteEp;
	std::unique_ptr<IPEndPoint> _localEp;
	SOCKET _sock;
public:
	char _AcceptexBuffer[(sizeof(SOCKADDR_IN)+16)*2] = "";
};