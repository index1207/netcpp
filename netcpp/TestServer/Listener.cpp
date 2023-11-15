#include "Listener.hpp"
#include <iostream>
#include <net/Netexcept.hpp>
#include "Listener.hpp"

using namespace std;

Listener::Listener() : _listenSock(Socket(AddressFamily::Internetwork, SocketType::Stream))
{
	if (!_listenSock.IsOpen())
		throw network_error("Invalid socket.");
}

Listener::Listener(IPEndPoint ep) : _listenSock(AddressFamily::Internetwork, SocketType::Stream)
{
	if (!_listenSock.IsOpen())
		throw network_error("Invalid socket.");

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
	cout << "Server is running on " << _listenSock.GetLocalEndPoint().ToString() << '\n';
	for (int i = 0; i < count; ++i) {
		auto acceptEvent = new AcceptEvent;
		_acceptEvents.push(acceptEvent);

		acceptEvent->completed = std::bind(&Listener::OnAcceptCompleted, this, std::placeholders::_1);
		StartAccept(acceptEvent);
	}
}

void Listener::Run(IPEndPoint ep, int count)
{
	if (!_listenSock.Bind(ep))
		throw network_error("Bind()");
	if (!_listenSock.Listen())
		throw network_error("Listen()");

	Run(count);
}

void Listener::StartAccept(AcceptEvent* event)
{
	if (!_listenSock.AcceptAsync(event)) {
		throw network_error("AcceptAsync()");
	}
}

void Listener::OnAcceptCompleted(net::SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success) {
		cout << "Connected!\n";
		StartAccept(static_cast<AcceptEvent*>(event));
	}
}
