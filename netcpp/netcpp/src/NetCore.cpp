#include "pch.h"
#include "NetCore.hpp"
#include "SocketAsyncEvent.hpp"
#include "Extension.hpp"

#include <iostream>

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
	Register(s->GetHandle());
}

void NetCore::Register(SOCKET s)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL);
}

void BindAcceptExSockAddress(AcceptEvent* args)
{
	SOCKADDR_IN* localAdr = nullptr,
		* remoteAdr = nullptr;
	int l_len = 0, r_len = 0;
	Extension::GetAcceptExSockaddrs(args->AcceptSocket->_AcceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAdr), &l_len,
		reinterpret_cast<SOCKADDR**>(&remoteAdr), &r_len);

	args->AcceptSocket->SetRemoteEndPoint(IPEndPoint::Parse(*remoteAdr));
	args->AcceptSocket->SetLocalEndPoint(IPEndPoint::Parse(*localAdr));
}

unsigned CALLBACK Worker(HANDLE hcp)
{
	while (true)
	{
		DWORD numOfBytes = 0;
		ULONG key = 0;
		SocketAsyncEvent* event = nullptr;
		if (!::GetQueuedCompletionStatus(hcp, &numOfBytes, reinterpret_cast<PULONG_PTR>(&key), reinterpret_cast<LPOVERLAPPED*>(&event), INFINITE))
		{
			continue;
		}

		event->socketError = Success;

		switch (event->eventType)
		{
			case EventType::Accept:
			{
				auto acceptEvent = static_cast<AcceptEvent*>(event);
				GNetCore.Register(acceptEvent->AcceptSocket->GetHandle());
				BindAcceptExSockAddress(acceptEvent);

				acceptEvent->completed(acceptEvent);
				break;
			}
			case EventType::Connect:
			{
				auto connectEvent = static_cast<ConnectEvent*>(event);
				connectEvent->completed(connectEvent);
				break;
			}
			case EventType::Send:
			{
				auto sendEvent = static_cast<SendEvent*>(event);
				sendEvent->completed(sendEvent);
				sendEvent->sentBytes = numOfBytes;
				break;
			}
			case EventType::Recv:
			{
				auto recvEvent = static_cast<RecvEvent*>(event);
				recvEvent->completed(recvEvent);
				break;
			}
		}
	}
}