#include <iostream>
#include <format>
#include <cassert>

#include <WinSock2.h>
#include <MSWSock.h>
#include <net/netcpp.hpp>

using namespace std;

unique_ptr<Socket> listenSock;

void PostAccept(SocketAsyncEventArgs* args);

void CompleteAccept(SocketAsyncEventArgs* args)
{
	if (args->SocketError == SocketError::Success)
	{
		cout << "Connected " << args->AcceptSocket->GetRemoteEndPoint()->ToString() << "\n";
	}
	else
		cout << format("AcceptAsync error. ({})\n", WSAGetLastError());

	PostAccept(args);
}

void PostAccept(SocketAsyncEventArgs* args)
{
	args->AcceptSocket = nullptr;

	bool pending = listenSock->AcceptAsync(args);
	if (!pending)
		CompleteAccept(args);
}

int main()
{
	listenSock = make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream);
	if (!listenSock->Bind(IPEndPoint(IPAddress::Any, 7777)))
		return -1;
	if (!listenSock->Listen(SOMAXCONN))
		return -1;

	std::cout << "Listening\n";

	SocketAsyncEventArgs args;
	args.Completed = CompleteAccept;
	PostAccept(&args);

	while (true)
	{
		this_thread::sleep_for(1s);
	}
	

	WSACleanup();
	return 0;
}