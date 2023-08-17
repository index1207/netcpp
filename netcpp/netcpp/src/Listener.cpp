#include "pch.h"
#include "Listener.hpp"


net::Listener::Listener(std::function<Agent*()> generateAgent)
	: _listenSock(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp),
	  _generateAgent(generateAgent)
{
}

net::Listener::Listener(IPEndPoint endPoint, std::function<Agent*()> generateAgent) : Listener(generateAgent)
{
	_ep = endPoint;
}

bool net::Listener::Run(int backlog)
{
	if (!_listenSock.Bind(_ep))
		return false;
	if (!_listenSock.Listen(backlog))
		return false;

	_acceptEvent.completed = [this](SocketAsyncEvent* args)
	{
		if (args->socketError == SocketError::Success)
		{
			auto acceptEvent = static_cast<AcceptEvent*>(args);
			auto agent = _generateAgent();
			agent->OnConnected();
			agent->Run(acceptEvent->acceptSocket);
		}
		PostAccept(static_cast<AcceptEvent*>(args));
	};
	PostAccept(&_acceptEvent);
}

bool net::Listener::Run(IPEndPoint endPoint, int backlog)
{
	_ep = endPoint;
	return Run(backlog);
}

void net::Listener::PostAccept(AcceptEvent* e)
{
	e->acceptSocket = nullptr;
	_listenSock.AcceptAsync(e);
}