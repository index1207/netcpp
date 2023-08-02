#pragma once

#include "Socket.hpp"

enum SocketError
{
	Success,
};

enum EventType
{
	None,
	Accept,
	Send,
	Recv
};

class SocketAsyncEventArgs : private WSAOVERLAPPED 
{
public:
	SocketAsyncEventArgs();
public:
	Socket AcceptSocket;
	SocketError SocketError;
	std::function<void(SocketAsyncEventArgs*)> Completed;
	EventType type;
};