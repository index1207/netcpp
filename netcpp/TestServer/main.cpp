#include <iostream>
#include <memory>
#include <thread>

#include <net/netcpp.hpp>

using namespace net;
using namespace std::chrono_literals;

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(Dns::GetHostEntry(Dns::GetHostName()).AddressList[0], 8085)))
		return -1;
	if (!listenSock.Listen())
		return -1;

	std::cout << "Listening " << listenSock.GetLocalEndPoint().ToString() << '\n';
	
	auto acceptEvent = std::make_shared<AcceptEvent>();
	acceptEvent->completed = [](SocketAsyncEvent* event)
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
	listenSock.AcceptAsync(acceptEvent);

	while (true)
	{
		std::this_thread::sleep_for(1s);
	}

	return 0;
}