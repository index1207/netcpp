#pragma once

#include <functional>

#include "Socket.hpp"

enum SocketError
{
	Success,
};

enum EventType
{
	Accept,
	Connect,
	Send,
	Recv
};


class SocketAsyncEvent : public OVERLAPPED
{
	void Init();
public:
	SocketAsyncEvent(EventType eventType);
	EventType eventType;
	SocketError socketError;
	std::function<void(SocketAsyncEvent*)> completed;
};

class AcceptEvent : public SocketAsyncEvent
{
public:
	AcceptEvent() : SocketAsyncEvent(EventType::Accept) { }
	Socket* acceptSocket;
};

class ConnectEvent : public SocketAsyncEvent
{
public:
	ConnectEvent() : SocketAsyncEvent(EventType::Connect) { }
	IPEndPoint endPoint;
};

class SendEvent : public SocketAsyncEvent
{
public:
	SendEvent() : SocketAsyncEvent(EventType::Send) { }
	ArraySegment segment;
	int sentBytes = 0;
};

class RecvEvent : public SocketAsyncEvent
{
public:
	RecvEvent() : SocketAsyncEvent(EventType::Recv) { }
	ArraySegment segment;
};