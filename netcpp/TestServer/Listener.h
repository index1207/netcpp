#pragma once
#include <net/Socket.hpp>
#include <net/SocketAsyncEvent.hpp>
#include <queue>

using namespace net;

class Listener
{
public:
	Listener(IPEndPoint ep);
	~Listener();
public:
	void Run(int count = 1);
private:
	void OnAcceptCompleted(SocketAsyncEvent* event);
private:
	Socket _listenSock;
	std::queue<AcceptEvent*> _acceptEvents;
};

