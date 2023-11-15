#include <iostream>
#include <memory>
#include <thread>

#include <net/netcpp.hpp>

using namespace net;
using namespace std::chrono_literals;

void OnAcceptCompleted(SocketAsyncEvent* event)
{
	std::cout << "Connected!\n";
	auto e = static_cast<AcceptEvent*>(event);
}

void AcceptStart(std::shared_ptr<AcceptEvent> acceptEvent, Socket& listenSock)
{
	acceptEvent->completed = OnAcceptCompleted;
	if (!listenSock.AcceptAsync(acceptEvent))
		OnAcceptCompleted(acceptEvent.get());
}

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(Dns::GetHostEntry(Dns::GetHostName()).AddressList[2], 8085)))
		return -1;
	if (!listenSock.Listen())
		return -1;

	std::cout << "Listening " << listenSock.GetLocalEndPoint().ToString() << '\n';

	auto acceptEvent = std::make_shared<AcceptEvent>();
	AcceptStart(acceptEvent, listenSock);

	getchar();
	
	return 0;
}