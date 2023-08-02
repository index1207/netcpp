#include <iostream>

#include <net/netcpp.hpp>

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

void PostAccept(SocketAsyncEventArgs* args);

void CompleteAccept(SocketAsyncEventArgs* args)
{
	if (args->SocketError == SocketError::Success)
	{
		cout << "Connected\n";
	}
	else
		cout << "AcceptEx Error.\n";

	listenSock.AcceptAsync(args);
}

void PostAccept(SocketAsyncEventArgs* args)
{
	args->AcceptSocket = Socket();

	bool pending = listenSock.AcceptAsync(args);
	if (!pending)
		CompleteAccept(args);
}

int main()
{
	Extension::Initialize();

	listenSock = Socket(AddressFamily::Internetwork, SocketType::Stream, ProtocolType::Tcp);
	listenSock.Bind(IPEndPoint(IPAddress::Parse("127.0.0.1"), 7777));
	listenSock.Listen(SOMAXCONN);

	std::cout << "Listening\n";

	auto hcp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	CreateIoCompletionPort((HANDLE)listenSock.GetHandle(), hcp, NULL, NULL);

	std::thread worker(WorkerMain, hcp);

	SocketAsyncEventArgs args;
	args.Completed = CompleteAccept;
	PostAccept(&args);

	worker.join();
	return 0;
}