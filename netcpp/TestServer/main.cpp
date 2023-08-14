#include <iostream>
#include <format>
#include <cassert>

#include <WinSock2.h>
#include <MSWSock.h>
#include <net/netcpp.hpp>

using namespace std;

unique_ptr<Socket> listenSock;

void PostAccept(AcceptEvent* args);

void OnCompletedReceive(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		auto recvEvent = static_cast<RecvEvent*>(event);
		cout << "Received " << recvEvent->segment.Array << '\n';
	}
	else
	{
		cout << format("ReceiveAsync error. ({})\n", WSAGetLastError());
	}
}

void OnCompletedAccept(SocketAsyncEvent* event)
{
	if (event->socketError == SocketError::Success)
	{
		auto acceptEvent = static_cast<AcceptEvent*>(event);
		cout << "Connected " << acceptEvent->AcceptSocket->GetRemoteEndPoint().ToString() << "\n";

		char buf[1024] = "";
		auto recvEvent = new RecvEvent;
		recvEvent->segment = ArraySegment(buf, 0, 1024);
		recvEvent->completed = OnCompletedReceive;
		if (!acceptEvent->AcceptSocket->ReceiveAsync(recvEvent))
			OnCompletedReceive(recvEvent);
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
		OnCompletedAccept(args);
}

int main()
{
	listenSock = make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream);
	if (!listenSock->Bind(IPEndPoint(IPAddress::Any, 8080)))
		return -1;
	if (!listenSock->Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening\n";

	for (int i = 0; i < 10; ++i)
	{
		auto acceptEvent = new AcceptEvent;
		acceptEvent->completed = OnCompletedAccept;
		PostAccept(acceptEvent);
	}

	while (true)
	{
		this_thread::sleep_for(1s);
	}
	
	return 0;
}