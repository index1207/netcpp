#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

Socket* sock;

void OnCompletedSend(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		auto sendEvent = static_cast<SendEvent*>(event);
		std::cout << "Sent " << sendEvent->sentBytes << " Bytes.\n";
	}
	else
	{
		std::cout << "Send Error\n";
	}
}


void OnCompletedConnect(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		std::cout << "Connected!\n";

		/*std::string data = "Hello";
		SendEvent* sendEvent = new SendEvent;
		sendEvent->completed = OnCompletedSend;
		sendEvent->segment = ArraySegment(data.data(), 0, data.size());
		if (!sock->SendAsync(sendEvent))
			OnCompletedSend(sendEvent);*/
	}
	else
	{
		std::cout << "Connect Error";
	}
}

int main()
{
	Sleep(500);
	for (int i = 0; i < 100; ++i)
	{
		auto sock = new Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
		if (!sock->IsValid())
			return -1;

		ConnectEvent args;
		args.endPoint = IPEndPoint(IPAddress::Loopback, 8080);
		args.completed = OnCompletedConnect;
		if (!sock->ConnectAsync(&args))
			OnCompletedConnect(&args);
		Sleep(1);
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::duration<double>(1));
	}
}