#include "pch.h"
#include "dispatcher.hpp"

#include <cassert>
#include <windef.h>

#include "io_context.hpp"
#include "..\net\wsock.hpp"

using namespace net;

dispatcher net::GNetCore;

dispatcher::dispatcher()
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

dispatcher::~dispatcher()
{
	CloseHandle(_hcp);
}

void dispatcher::push(socket& sock)
{
	push(sock.get_handle());
}

void dispatcher::push(SOCKET s)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL);
}

void BindAcceptExSockAddress(context::accept* args)
{
	SOCKADDR_IN* localAdr = nullptr,
		* remoteAdr = nullptr;
	int l_len = 0, r_len = 0;
	wsock::GetAcceptExSockaddrs(args->_acceptexBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAdr), &l_len,
		reinterpret_cast<SOCKADDR**>(&remoteAdr), &r_len);

	args->acceptSocket->set_remote(endpoint::Parse(*remoteAdr));
	args->acceptSocket->set_local(endpoint::Parse(*localAdr));
}


unsigned CALLBACK net::worker(HANDLE hcp)
{
	while (true)
	{
		DWORD transferredBytes = 0;
		ULONG_PTR agent = 0;
		io_context* event = nullptr;
		if (!::GetQueuedCompletionStatus(hcp, &transferredBytes, &agent, reinterpret_cast<LPOVERLAPPED*>(&event), INFINITE))
		{
			continue;
		}

		event->socketError = socket_error::SUCCESS;
		
		switch (event->eventType)
		{
			case context_type::ACCEPT:
			{
				auto acceptEvent(static_cast<context::accept*>(event));
				GNetCore.push(acceptEvent->acceptSocket->get_handle());

				BindAcceptExSockAddress(acceptEvent);
				acceptEvent->completed(acceptEvent);
				break;
			}
			case context_type::CONNECT:
			{
				auto connectEvent(static_cast<context::connect*>(event));
				connectEvent->completed(connectEvent);
				break;
			}
			case context_type::DISCONNECT:
			{
				event->completed(event);
				break;
			}
			case context_type::SEND:
			{
				auto sendEvent(static_cast<context::send*>(event));
				sendEvent->sentBytes = transferredBytes;
				sendEvent->completed(sendEvent);
				break;
			}
			case context_type::RECEIVE:
			{
				auto recvEvent(static_cast<context::receive*>(event));

				recvEvent->recvBytes = transferredBytes;
				recvEvent->completed(recvEvent);
				break;
			}
		}
	}
}