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
	if(INVALID_HANDLE_VALUE == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL))
        throw network_error("CreateIoCompletionPort");
}

unsigned CALLBACK net::worker(HANDLE hcp)
{
	while (true)
	{
		DWORD transferredBytes = 0;
		ULONG_PTR agent = 0;
		Context* context = nullptr;
		if (::GetQueuedCompletionStatus(hcp, &transferredBytes, &agent, reinterpret_cast<LPOVERLAPPED*>(&context), INFINITE))
        {
            switch (context->contextType) {
                case ContextType::Accept: {
                    Socket &listenSock = *reinterpret_cast<Socket *>(context->token);
                    context->acceptSocket->setSocketOption(OptionLevel::Socket, OptionName::UpdateAcceptContext,
                                                           listenSock.getHandle());
                    context->acceptSocket->BindEndpoint(listenSock.getLocalEndpoint());
                    break;
                }
                case ContextType::Connect:
                    context->acceptSocket->setSocketOption(OptionLevel::Socket, OptionName::UpdateConnectContext, NULL);
                    break;
                case ContextType::Disconnect:
                    break;
                case ContextType::Send:
                case ContextType::Receive: {
                    context->length = transferredBytes;
                    break;
                }
            }
            context->completed(context);
        }
        else
        {
            printf("%d\n", WSAGetLastError());
        }
    }
}