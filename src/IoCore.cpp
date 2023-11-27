#include "PCH.h"
#include "IoCore.hpp"

#include <cassert>
#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"

using namespace net;

IoCore net::g_dispatcher;

IoCore::IoCore()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	for (unsigned i = 0; i < info.dwNumberOfProcessors; ++i)
	{
		auto thread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, NULL, worker, _hcp, 0, NULL));
		if(thread == nullptr)
			throw std::runtime_error("Failed create thread.");
	}
}

IoCore::~IoCore()
{
	CloseHandle(_hcp);
}

void IoCore::push(Socket& sock)
{
	push(sock.getHandle());
}

void IoCore::push(SOCKET s)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL);
}

void BindAcceptExSockAddress(AcceptContext* args)
{
	SOCKADDR_IN* localAdr = nullptr,
		* remoteAdr = nullptr;
	int l_len = 0, r_len = 0;
	Native::GetAcceptExSockaddrs(args->_buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                                 reinterpret_cast<SOCKADDR**>(&localAdr), &l_len,
                                 reinterpret_cast<SOCKADDR**>(&remoteAdr), &r_len);

    args->accept_socket->setRemoteEndpoint(Endpoint::parse(*remoteAdr));
    args->accept_socket->setLocalEndpoint(Endpoint::parse(*localAdr));
}


unsigned CALLBACK net::worker(HANDLE hcp)
{
	while (true)
	{
		DWORD transferredBytes = 0;
		ULONG_PTR agent = 0;
		Context* event = nullptr;
		if (!::GetQueuedCompletionStatus(hcp, &transferredBytes, &agent, reinterpret_cast<LPOVERLAPPED*>(&event), INFINITE))
		{
			continue;
		}

		event->socketError = SocketError::Success;
		
		switch (event->eventType)
		{
			case ContextType::Accept:
			{
				auto acceptEvent(static_cast<AcceptContext*>(event));
				g_dispatcher.push(acceptEvent->accept_socket->getHandle());

				BindAcceptExSockAddress(acceptEvent);
				acceptEvent->completed(acceptEvent);
				break;
			}
			case ContextType::Connect:
			{
				auto connectEvent(static_cast<ConnectContext*>(event));
				connectEvent->completed(connectEvent);
				break;
			}
			case ContextType::Disconnect:
			{
				event->completed(event);
				break;
			}
			case ContextType::Send:
			{
				auto sendEvent(static_cast<SendContext*>(event));
				sendEvent->length = transferredBytes;
				sendEvent->completed(sendEvent);
				break;
			}
			case ContextType::Receive:
			{
				auto recvEvent(static_cast<ReceiveContext*>(event));

				recvEvent->length = transferredBytes;
				recvEvent->completed(recvEvent);
				break;
			}
		}
	}
}