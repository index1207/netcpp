#include "pch.h"
#include "Agent.hpp"

using namespace net;

net::Agent::~Agent()
{
}

Socket net::Agent::GetSocket()
{
	return *_sock;
}

void net::Agent::Run(std::shared_ptr<Socket> s)
{
	_isDisconnect = false;
	_sock = s;

	_recvEvent.segment = net::ArraySegment(new char[128] {""}, 0, 128);
	_recvEvent.completed = [this](SocketAsyncEvent* args)
	{
		if (args->socketError == SocketError::Success)
		{
			auto recvEvent = static_cast<RecvEvent*>(args);
			if (recvEvent->recvBytes == 0)
				Disconnect();

			OnRecv(recvEvent->segment.Array, recvEvent->recvBytes);

			PostRecv(static_cast<RecvEvent*>(args));
		}
		else
			Disconnect();
	};
	PostRecv(&_recvEvent);
}

void net::Agent::Disconnect()
{
	if (!_isDisconnect)
	{
		_sock->Disconnect();
		OnDisconnected();

		_isDisconnect.store(true);
	}
}

void net::Agent::PostRecv(RecvEvent* recvEvent)
{
	if (!_sock->ReceiveAsync(recvEvent))
		Disconnect();
}
