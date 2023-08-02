#include <iostream>
#include <format>
#include <net/netcpp.hpp>

using namespace std;

unique_ptr<Socket> listenSock;

void PostAccept(SocketAsyncEventArgs* args);

void CompleteAccept(SocketAsyncEventArgs* args)
{
	if (args->SocketError == SocketError::Success)
	{
		cout << "Connected " << args->AcceptSocket->GetHandle() << "\n";
	}
	else
		cout << format("AcceptEx Error. ({})\n", WSAGetLastError());

	listenSock->AcceptAsync(args);
}

void PostAccept(SocketAsyncEventArgs* args)
{
	bool pending = listenSock->AcceptAsync(args);
	if (!pending)
		CompleteAccept(args);
}

int main()
{
	listenSock = make_unique<Socket>(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	listenSock->Bind(IPEndPoint(IPAddress::Parse("127.0.0.1"), 7777));
	listenSock->Listen(SOMAXCONN);

	std::cout << "Listening\n";

	SocketAsyncEventArgs args;
	args.Completed = CompleteAccept;
	PostAccept(&args);

	while (true)
	{
		this_thread::sleep_for(1s);
	}
	
	return 0;
}