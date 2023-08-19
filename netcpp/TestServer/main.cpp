#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

using namespace net;

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(Dns::GetHostEntry(Dns::GetHostName()).AddressList[0], 8080)))
		return -1;
	if (!listenSock.Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening " << listenSock.GetLocalEndPoint().ToString() << '\n';

	AcceptEvent acceptEvent;
	acceptEvent.completed = [](SocketAsyncEvent* event)
		{
			if (event->socketError == SocketError::Success)
			{
				std::cout << "Accepted!\n";
			}
			else
			{
				std::cout << "Accept Error\n";
			}
		};
	listenSock.AcceptAsync(&acceptEvent);

	while (true)
	{
		Sleep(1000);
	}

	return 0;
}