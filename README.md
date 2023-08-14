# netcpp
netcpp is a simple networking library. It is written in C++ and is based on IOCP.

## Example
```cpp
// Server
#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

int main()
{
	Socket listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!listenSock.Bind(IPEndPoint(IPAddress::Any, 8080)))
		return -1;
	if (!listenSock.Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening\n";

	AcceptEvent acceptEvent;
	acceptEvent.completed = [](SocketAsyncEvent* acceptEvent)
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
		this_thread::sleep_for(1s);
	}
	
	return 0;
}
```
```cpp
// Client
#include <iostream>
#include <thread>

#include <net/netcpp.hpp>

int main()
{
	Socket sock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	if (!sock.IsValid())
		return -1;

	ConnectEvent args;
	args.endPoint = IPEndPoint(IPAddress::Loopback, 8080);
	args.completed = [](SocketAsyncEvent* event)
    {
        if (event->socketError == SocketError::Success)
        {
            std::cout << "Connected!\n";
        }
        else
        {
            std::cout << "Connect Error\n";
        }
    };
	sock.ConnectAsync(&args);

	while (true)
	{
		this_thread::sleep_for(1s);
	}

    return 0;
}
```