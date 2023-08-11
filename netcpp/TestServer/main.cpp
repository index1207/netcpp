#include <iostream>
#include <format>
#include <cassert>

#include <WinSock2.h>
#include <MSWSock.h>
#include <net/netcpp.hpp>

using namespace std;

unique_ptr<Socket> listenSock;

void PostAccept(AcceptEvent* args);

void CompleteAccept(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		AcceptEvent* acceptEvent = (AcceptEvent*)event;
		cout << "Connected " << acceptEvent->AcceptSocket->GetRemoteEndPoint()->ToString() << "\n";
	}
	else
		cout << format("AcceptAsync error. ({})\n", WSAGetLastError());

	PostAccept((AcceptEvent*)event);
}

void PostAccept(AcceptEvent* args)
{
	args->AcceptSocket = nullptr;

	bool pending = listenSock->AcceptAsync(args);
	if (!pending)
		CompleteAccept(args);
}

int main()
{
	listenSock = make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream);
	if (!listenSock->Bind(IPEndPoint(IPAddress::Parse("192.168.0.67"), 7777)))
		return -1;
	if (!listenSock->Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening\n";

	AcceptEvent acceptEvent;
	acceptEvent.Completed = CompleteAccept;
	PostAccept(&acceptEvent);

	while (true)
	{
		this_thread::sleep_for(1s);
	}
	

	WSACleanup();
	return 0;
}