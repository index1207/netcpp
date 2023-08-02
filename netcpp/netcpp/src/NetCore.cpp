#include "pch.h"
#include "NetCore.hpp"
#include "Socket.hpp"
#include "SocketAsyncEventArgs.hpp"
#include "Extension.hpp"

class netcpp final
{
public:
	netcpp()
	{
		WSADATA wsaData;
		assert(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

		Extension::Initialize();
	}
	~netcpp()
	{
		WSACleanup();
	}
} _netcpp{};

unsigned CALLBACK Worker(HANDLE hcp)
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
			GNetCore.Register(args->AcceptSocket.get());
			break;
		}
		args->Completed(args);
	}
}

NetCore GNetCore;

NetCore::NetCore()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	HANDLE thread = (HANDLE)::_beginthreadex(NULL, NULL, Worker, _hcp, 0, NULL);
}

NetCore::~NetCore()
{
	CloseHandle(_hcp);
}

void NetCore::Register(class Socket* s)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s->GetHandle()), _hcp, NULL, NULL);
}