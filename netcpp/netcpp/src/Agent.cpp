#include "pch.h"
#include "Agent.hpp"
#include "Socket.hpp"

using namespace net;

net::Agent::Agent(Socket* s)
{
	_sock = s;
}

net::Agent::~Agent()
{
}

Socket net::Agent::GetSocket()
{
	return *_sock;
}
