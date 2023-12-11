#include "PCH.h"
#include "IoSystem.hpp"

#include <thread>

#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"

using namespace net;

IoSystem net::ioSystem;

IoSystem::IoSystem()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	for (unsigned i = 0; i < info.dwNumberOfProcessors; ++i)
	{
        auto t = new std::thread(worker, _hcp);
        _workers.emplace_back(t);
    }
}

IoSystem::~IoSystem()
{
	CloseHandle(_hcp);
}

void IoSystem::push(Socket& sock)
{
	push(sock.getHandle());
}

void IoSystem::push(SOCKET s)
{
	if(INVALID_HANDLE_VALUE == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL))
        throw network_error("CreateIoCompletionPort");
}

unsigned CALLBACK IoSystem::worker(HANDLE hcp)
{
    auto dispatch = [](Context* context, DWORD transferredBytes) mutable {
        if (context != nullptr) {
            switch (context->_contextType) {
            case ContextType::Accept:
                context->acceptSocket->setSocketOption(OptionLevel::Socket, OptionName::UpdateAcceptContext, context->_sock);
                break;
            case ContextType::Connect:
                if (0 != context->_sock->setSocketOption(OptionLevel::Socket, OptionName::UpdateConnectContext, NULL));
                break;
            case ContextType::Disconnect:
                break;
            case ContextType::Send:
            case ContextType::Receive:
                context->length = transferredBytes;
                break;
            default:
                break;
            }
            if (context->completed != nullptr)
                context->completed(context);
        }
    };
    while (true)
    {
        DWORD transferredBytes = 0;
        ULONG_PTR key = 0;
        Context* context = nullptr;
        if (::GetQueuedCompletionStatus(hcp, &transferredBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&context), INFINITE))
        {
            dispatch(context, transferredBytes);
            //printf("Completed Work!");
        }
    }
}