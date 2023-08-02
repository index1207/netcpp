#include "pch.h"
#include <SocketAsyncEventArgs.hpp>

SocketAsyncEventArgs::SocketAsyncEventArgs()
{
	memset(this, 0, sizeof(WSAOVERLAPPED));

	this->SocketError = (::SocketError)-1;
	type = EventType::None;
}
