#pragma once

#include "Socket.hpp"

enum SocketError
{
	Success = 0
};

class SocketAsyncEventArgs : public WSAOVERLAPPED 
{
public:
	Socket* acceptSocket;
	SocketError socketError;
};