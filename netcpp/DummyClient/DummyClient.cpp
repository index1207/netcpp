#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

void CompleteConnect(SocketAsyncEventArgs* args)
{
	if (args->SocketError == SocketError::Success)
	{
		std::cout << "Connected!\n";
	}
	else
	{
		std::cout << "Connect Error\n";
	}
}

int main()
{
	std::this_thread::sleep_for(std::chrono::duration<double>(0.1));
	auto sock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!sock.IsValid())
		return -1;

	SocketAsyncEventArgs args;
	args.EndPoint = IPEndPoint(IPAddress::Loopback, 7777);
	args.Completed = CompleteConnect;
	if (!sock.ConnectAsync(&args))
		CompleteConnect(&args);

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::duration<double>(1));
	}
}