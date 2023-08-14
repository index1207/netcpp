#include "pch.h"
#include "SocketAsyncEvent.hpp"

using namespace net;

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
