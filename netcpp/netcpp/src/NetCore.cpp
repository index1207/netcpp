#include "pch.h"
#include "NetCore.hpp"
#include "SocketAsyncEventArgs.hpp"
#include "Extension.hpp"

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

void BindAcceptExSockAddress(SocketAsyncEventArgs* args)
{
	IPAddress* localAdr = nullptr,
		* remoteAdr = nullptr;
	int l_len = 0, r_len = 0;
	Extension::GetAcceptExSockaddrs(&args->AcceptSocket->_AcceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAdr), &l_len,
		reinterpret_cast<SOCKADDR**>(&remoteAdr), &r_len);

	args->AcceptSocket->SetRemoteEndPoint(IPEndPoint(*remoteAdr, ntohs(reinterpret_cast<SOCKADDR_IN*>(remoteAdr)->sin_port)));
	args->AcceptSocket->SetLocalEndPoint(IPEndPoint(*localAdr, ntohs(reinterpret_cast<SOCKADDR_IN*>(localAdr)->sin_port)));
}

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
				// Disconnect
			continue;
		}
		else
			args->SocketError = SocketError::Success;

		switch (args->type)
		{
		case EventType::Accept:
			GNetCore.Register(args->AcceptSocket.get());
			BindAcceptExSockAddress(args);
			args->Completed(args);
			break;
		case EventType::Connect:
			args->Completed(args);
			break;
		}
	}
}