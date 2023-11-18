#include "pch.h"
#include "io_context.hpp"

using namespace net;

void io_context::Init()
{
	memset(this, 0, sizeof(WSAOVERLAPPED));
	this->socketError = socket_error::ERROR;
}

io_context::io_context(context_type eventType)
{
	Init();
	this->eventType = eventType;
}
