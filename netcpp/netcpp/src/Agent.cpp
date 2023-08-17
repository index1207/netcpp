#include "pch.h"
#include "Agent.hpp"

using namespace net;

net::Agent::Agent(Socket s) : _sock(s)
{
}

net::Agent::~Agent()
{
}

Socket net::Agent::GetSocket()
{
	return _sock;
}

void net::Agent::Run(Socket s)
{
	_sock = s;

	_recvEvent.completed = [this](SocketAsyncEvent* args)
	{
		if (args->socketError == SocketError::Success)
		{
			auto recvEvent = static_cast<RecvEvent*>(args);
			if (OnRecv(recvEvent->segment.Array, recvEvent->recvBytes) == 0)
				Disconnect();

			PostRecv(static_cast<RecvEvent*>(args));
		}
		else
			Disconnect();
	};
	PostRecv(&_recvEvent);
}

void net::Agent::Disconnect()
{
	_sock.Disconnect();
}

void net::Agent::PostRecv(RecvEvent* recvEvent)
{
}
