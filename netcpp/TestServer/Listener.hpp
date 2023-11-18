#pragma once
#include <net/socket.hpp>
#include <net/io_context.hpp>
#include <queue>

using namespace net;

class Listener
{
public:
	Listener();
	Listener(endpoint ep);
	~Listener();
public:
	void Run(int count = 1);
	void Run(endpoint ep, int count = 1);
private:
	void StartAccept(context::accept* event);
	void OnAcceptCompleted(io_context* event);
private:
	net::socket _listenSock;
	std::queue<context::accept*> _acceptEvents;
};

