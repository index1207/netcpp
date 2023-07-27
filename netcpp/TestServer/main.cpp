#include <iostream>

#include <WinSock2.h>
#include "../netcpp/net/CorePch.hpp"
#include "../netcpp/net/Socket.hpp"
#include "../netcpp/net/SocketEx.hpp"
#include "../netcpp/net/SocketAsyncEventArgs.hpp"

using namespace std;

void PostAccept(Socket& sock, SocketAsyncEventArgs& args);

void CompleteAccept(void* sender, Socket& sock, SocketAsyncEventArgs& args)
{
	if (args.socketError == SocketError::Success)
	{
		cout << "Connected!\n";
	}
	
	PostAccept(sock, args);
}

void PostAccept(Socket& sock, SocketAsyncEventArgs& args)
{
	args.acceptSocket = nullptr;

	bool pending = sock.AcceptAsync(&args);
	if (!pending)
		CompleteAccept(nullptr, sock, args);
}

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

	}

	SocketEx::Initialize();

	auto sock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	sock.Bind(IPEndPoint(IPAddress::Any, 7777));
	sock.Listen(4);

	SocketAsyncEventArgs args;
	PostAccept(sock, args);

	WSACleanup();
}