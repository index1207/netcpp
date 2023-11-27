#include "PCH.h"
#include "Context.hpp"

using namespace net;

void Context::Init()
{
	memset(this, 0, sizeof(WSAOVERLAPPED));
	this->socketError = SocketError::Error;
}

Context::Context(ContextType eventType)
{
	Init();
	this->eventType = eventType;
}
