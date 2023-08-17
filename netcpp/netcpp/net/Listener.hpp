#pragma once

#include "IPEndPoint.hpp"
#include "Agent.hpp"
#include "SocketAsyncEvent.hpp"

namespace net
{
	class Listener
	{
	public:
		Listener(std::function<Agent*()> generateAgent);
		Listener(IPEndPoint endPoint, std::function<Agent*()> generateAgent);
	public:
		bool Run(int backlog = SOMAXCONN);
		bool Run(IPEndPoint endPoint, int backlog	= SOMAXCONN);
	private:
		void PostAccept(AcceptEvent* e);
	private:
		IPEndPoint _ep;
		Socket _listenSock;
		std::function<Agent*()> _generateAgent;
		AcceptEvent _acceptEvent;
	};
}

