#include <iostream>

#include <WinSock2.h>
#include "../netcpp/net/CorePch.hpp"
#include "../netcpp/net/Socket.hpp"
#include "../netcpp/net/SocketEx.hpp"
#include "../netcpp/net/SocketAsyncEventArgs.hpp"

using namespace std;

void WorkerMain(HANDLE hcp)
{
	while (true)
	{
		DWORD numOfBytes = 0;
		ULONG key = 0;
		SocketAsyncEventArgs* args = nullptr;
		if (!::GetQueuedCompletionStatus(hcp, &numOfBytes, reinterpret_cast<PULONG_PTR>(&key), reinterpret_cast<LPOVERLAPPED*>(&args), INFINITE))
		{
			if (numOfBytes == 0)
				std::cout << "Disconnected\n";
			continue;
		}
		else
			args->SocketError = SocketError::Success;

		switch (args->type)
		{
		case EventType::Accept:
			CreateIoCompletionPort((HANDLE)args->AcceptSocket.GetHandle(), hcp, NULL, NULL);
			break;
		}
		args->Completed(args);
	}
}

Socket listenSock;

void OnConnect(SocketAsyncEventArgs* args)
{
	if (args->SocketError == SocketError::Success)
	{
		cout << "Connected\n";
	}
	else
		cout << "AcceptEx Error.\n";

	listenSock.AcceptAsync(args);
}

int main()
{
	SocketEx::Initialize();

	listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	listenSock.Bind(IPEndPoint(IPAddress::Any, 7777));
	listenSock.Listen(4);

	std::cout << "Listening\n";

	auto hcp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	CreateIoCompletionPort((HANDLE)listenSock.GetHandle(), hcp, NULL, NULL);

	std::thread worker(WorkerMain, hcp);

	SocketAsyncEventArgs* args = new SocketAsyncEventArgs();
	args->Completed = OnConnect;
	bool pending = listenSock.AcceptAsync(args);
	if (!pending)
		OnConnect(args);

	while (true)
	{
		this_thread::sleep_for(100ms);
	}

	worker.join();

	return 0;
}