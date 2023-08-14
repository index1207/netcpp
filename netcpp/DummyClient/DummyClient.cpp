#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

void OnCompletedConnect(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		std::cout << "Connected!\n";
	}
	else
	{
		std::cout << "Connect Error\n";
	}
}

void OnCompletedSend(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		auto sendEvent = static_cast<SendEvent*>(event);
		std::cout << "Sent " << sendEvent->sentBytes << "Bytes.\n";
	}
	else
	{
		std::cout << "Connect Error\n";
	}
}

int main()
{
	Sleep(500);
	auto sock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!sock.IsValid())
		return -1;

	ConnectEvent args;
	args.EndPoint = IPEndPoint(IPAddress::Loopback, 8080);
	args.completed = OnCompletedConnect;
	if (!sock.ConnectAsync(&args))
		OnCompletedConnect(&args);
	
	std::string data = "Hello";
	SendEvent sendEvent;
	sendEvent.completed = OnCompletedSend;
	sendEvent.segment = ArraySegment(data.data(), 0, data.size());
	if (!sock.SendAsync(&sendEvent))
		OnCompletedSend(&sendEvent);

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::duration<double>(1));
	}
}