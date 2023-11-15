#include "Listener.h"
#include <iostream>
#include <net/Netexcep.h>

using namespace std;

Listener::Listener(IPEndPoint ep)
{
	_listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream);
	if (!_listenSock.IsValid())
		throw network_error("Invalid socket.");

	cout << ep.ToString() << '\n';
	if (!_listenSock.Bind(ep))
		throw network_error("Bind()");
	if(!_listenSock.Listen())
		throw network_error("Listen()");
}

Listener::~Listener()
{
	while (!_acceptEvents.empty()) {
		delete _acceptEvents.front();
		_acceptEvents.pop();
	}
}

void Listener::Run(int count)
{
	for (int i = 0; i < count; ++i) {
		auto acceptEvent = new AcceptEvent;
		acceptEvent->completed = std::bind(&Listener::OnAcceptCompleted, this, std::placeholders::_1);

		if (_listenSock.AcceptAsync(acceptEvent)) {
			throw network_error("AcceptAsync()");
		}
		_acceptEvents.push(acceptEvent);
	}
}

void Listener::OnAcceptCompleted(net::SocketAsyncEvent* event)
{
	cout << "Connected!\n";
}
