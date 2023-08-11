#include "pch.h"
#include <SocketAsyncEvent.hpp>

void SocketAsyncEvent::Init()
{
	memset(this, 0, sizeof(WSAOVERLAPPED));
	this->socketError = (::SocketError)-1;
}

SocketAsyncEvent::SocketAsyncEvent(EventType eventType)
{
	Init();
	this->eventType = eventType;
}
