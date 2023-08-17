#include "pch.h"
#include "NetCore.hpp"
#include "SocketAsyncEvent.hpp"
#include "Extension.hpp"
#include "Agent.hpp"

#ifdef USE_AGENT
using CompletionKey = Agent;
#else
using CompletionKey = ULONG_PTR;
#endif


using namespace net;

NetCore net::GNetCore;

NetCore::NetCore()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	//for (int i = 0; i < info.dwNumberOfProcessors * 2; ++i)
	{
		HANDLE thread = (HANDLE)::_beginthreadex(NULL, NULL, Worker, _hcp, 0, NULL);
		assert(thread);
	}
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

void net::NetCore::Register(Agent* agent)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(agent->GetSocket().GetHandle()), _hcp, (ULONG_PTR)agent, NULL);
}

void BindAcceptExSockAddress(AcceptEvent* args)
{
	SOCKADDR_IN* localAdr = nullptr,
		* remoteAdr = nullptr;
	int l_len = 0, r_len = 0;
	Extension::GetAcceptExSockaddrs(args->_acceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAdr), &l_len,
		reinterpret_cast<SOCKADDR**>(&remoteAdr), &r_len);

	args->acceptSocket->SetRemoteEndPoint(IPEndPoint::Parse(*remoteAdr));
	args->acceptSocket->SetLocalEndPoint(IPEndPoint::Parse(*localAdr));
}


unsigned CALLBACK net::Worker(HANDLE hcp)
{
	while (true)
	{
		DWORD transfferredBytes = 0;
		CompletionKey* agent = nullptr;
		SocketAsyncEvent* event = nullptr;
		if (!::GetQueuedCompletionStatus(hcp, &transfferredBytes, reinterpret_cast<PULONG_PTR>(&agent), reinterpret_cast<LPOVERLAPPED*>(&event), INFINITE))
		{
			continue;
		}

		event->socketError = Success;

		switch (event->eventType)
		{
			case EventType::Accept:
			{
				auto acceptEvent = static_cast<AcceptEvent*>(event);
#ifdef USE_AGENT
				GNetCore.Register(agent);
#else
				GNetCore.Register(acceptEvent->acceptSocket->GetHandle());
#endif // USE_AGENT

				BindAcceptExSockAddress(acceptEvent);
				acceptEvent->completed(acceptEvent);
				break;
			}
			case EventType::Connect:
			{
				auto connectEvent = static_cast<ConnectEvent*>(event);
#ifdef USE_AGENT
				agent->OnConnected();
#endif
				connectEvent->completed(connectEvent);
				break;
			}
			case EventType::Send:
			{
				auto sendEvent = static_cast<SendEvent*>(event);
				sendEvent->sentBytes = transfferredBytes;
				sendEvent->completed(sendEvent);
				break;
			}
			case EventType::Recv:
			{
				auto recvEvent = static_cast<RecvEvent*>(event);
#ifdef USE_AGENT
				if (transfferredBytes == 0)
					agent->OnDisconnected();
				else
					agent->OnRecv(recvEvent->segment.Array, transfferredBytes);
#endif // USE_AGENT

				recvEvent->recvBytes = transfferredBytes;
				recvEvent->completed(recvEvent);
				break;
			}
		}
	}
}