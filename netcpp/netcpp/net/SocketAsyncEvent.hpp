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
	std::function<void(SocketAsyncEvent*)> Completed;
};

class AcceptEvent : public SocketAsyncEvent
{
public:
	AcceptEvent() : SocketAsyncEvent(EventType::Accept) { }
	std::unique_ptr<Socket> AcceptSocket;
};

class ConnectEvent : public SocketAsyncEvent
{
public:
	ConnectEvent() : SocketAsyncEvent(EventType::Connect) { }
	IPEndPoint EndPoint;
};