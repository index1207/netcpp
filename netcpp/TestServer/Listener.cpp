#include "Listener.hpp"
#include <iostream>
#include <net\exception.hpp>

using namespace std;

Listener::Listener() : _listenSock(net::socket(address_family::IPV4, socket_type::STREAM))
{
	if (!_listenSock.valid())
		throw network_error("Invalid socket.");
}

Listener::Listener(endpoint ep) : _listenSock(address_family::IPV4, socket_type::STREAM)
{
	if (!_listenSock.valid())
		throw network_error("Invalid socket.");

	if (!_listenSock.bind(ep))
		throw network_error("bind()");
	if(!_listenSock.listen())
		throw network_error("listen()");
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
	cout << "Server is running on " << _listenSock.get_local().ToString() << '\n';
	for (int i = 0; i < count; ++i) {
		auto acceptEvent = new context::accept;
		_acceptEvents.push(acceptEvent);

		acceptEvent->completed = std::bind(&Listener::OnAcceptCompleted, this, std::placeholders::_1);
		StartAccept(acceptEvent);
	}
}

void Listener::Run(endpoint ep, int count)
{
	if (!_listenSock.bind(ep))
		throw network_error("bind()");
	if (!_listenSock.listen())
		throw network_error("listen()");

	Run(count);
}

void Listener::StartAccept(context::accept* event)
{
	if (!_listenSock.accept_async(event)) {
		throw network_error("accept_async()");
	}
}

void Listener::OnAcceptCompleted(net::io_context* event)
{
	if (event->socketError == socket_error::SUCCESS) {
		cout << "Connected!\n";

		auto acceptEvent = static_cast<context::accept*>(event);
		StartAccept(acceptEvent);
	}
}
