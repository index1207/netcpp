#pragma once

#include <functional>

#include "Socket.hpp"

enum SocketError
{
	Success,
};

enum EventType
{
	None,
	Accept,
	Connect,
	Send,
	Recv
};

class SocketAsyncEventArgs : private WSAOVERLAPPED 
{
public:
	SocketAsyncEventArgs();
public:
	SocketError SocketError;
	std::function<void(SocketAsyncEventArgs*)> Completed;
	EventType type;
	std::unique_ptr<Socket> AcceptSocket;
	IPEndPoint EndPoint;
};