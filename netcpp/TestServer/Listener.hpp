#pragma once
#include <net/Socket.hpp>
#include <net/SocketAsyncEvent.hpp>
#include <queue>

using namespace net;

class Listener
{
public:
	Listener();
	Listener(IPEndPoint ep);
	~Listener();
public:
	void Run(int count = 1);
	void Run(IPEndPoint ep, int count = 1);
private:
	void StartAccept(AcceptEvent* event);
	void OnAcceptCompleted(SocketAsyncEvent* event);
private:
	Socket _listenSock;
	std::queue<AcceptEvent*> _acceptEvents;
};

