// Server
#include <iostream>
#include <thread>
#include <net/netcpp.hpp>

using namespace net;

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(IPAddress::Any, 8080)))
		return -1;
	if (!listenSock.Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening\n";

	AcceptEvent acceptEvent;
	acceptEvent.completed = [](SocketAsyncEvent* event)
	{
		if (event->socketError == SocketError::Success)
		{
			std::cout << "Accepted!\n";

			auto acceptEvent = static_cast<AcceptEvent*>(event);
			auto disconnectEvent = new DisconnectEvent;
			disconnectEvent->completed = [](SocketAsyncEvent* disconn)
			{
				std::cout << "Disconnected";
			};
			acceptEvent->acceptSocket->DisconnectAsync(disconnectEvent);
		}
		else
		{
			std::cout << "Accept Error\n";
		}
	};
	listenSock.AcceptAsync(&acceptEvent);

	while (true)
	{
		Sleep(100);
	}

	return 0;
}