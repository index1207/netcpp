#include "pch.h"
#include "net/SocketAsyncEventArgs.hpp"

SocketAsyncEventArgs::SocketAsyncEventArgs()
{
	memset(this, 0, sizeof(WSAOVERLAPPED));
	this->SocketError = (::SocketError)-1;
}
