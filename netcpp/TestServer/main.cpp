#include <iostream>
#include <memory>
#include <thread>

#include <net/netcpp.hpp>

using namespace net;
using namespace std::chrono_literals;

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(Dns::GetHostEntry(Dns::GetHostName()).AddressList[2], 8085)))
		return -1;
	if (!listenSock.Listen())
		return -1;

	auto acceptEvent = std::make_shared<AcceptEvent>();
	std::cout << "Listening " << listenSock.GetLocalEndPoint().ToString() << '\n';
	acceptEvent->completed = [&](SocketAsyncEvent* event)
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

	getchar();
	
	return 0;
}