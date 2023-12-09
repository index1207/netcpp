#include "PCH.h"
#include "IoCore.hpp"

#include <future>

#include <cassert>
#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"

using namespace net;

IoCore net::ioCore;
std::mutex mtx;

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

unsigned CALLBACK IoCore::worker(HANDLE hcp)
{
    DWORD transferredBytes = 0;
    ULONG_PTR key = 0;
    Context* context = nullptr;
    auto dispatch = [&context, &transferredBytes]() mutable {
        context->isSuccess.store(true);
        switch (context->_contextType) {
            case ContextType::Accept: {
                context->acceptSocket->setSocketOption(OptionLevel::Socket, OptionName::UpdateAcceptContext, context->_sock);
                //context->acceptSocket->BindEndpoint();
                break;
            }
            case ContextType::Connect:
                if(0 != context->_sock->setSocketOption(OptionLevel::Socket, OptionName::UpdateConnectContext, NULL))
                    context->isSuccess.store(false);
                break;
            case ContextType::Disconnect:
                break;
            case ContextType::Send:
            case ContextType::Receive:
                context->length.store(transferredBytes);
                break;
            default:
                break;
            }
            if (context->completed)
                std::async(std::launch::async, context->completed, context).wait();
                //context->completed(context);
    };
	while (true)
	{
		if (::GetQueuedCompletionStatus(hcp, &transferredBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&context), INFINITE))
        {
            dispatch();
        }
        else
        {
            switch(WSAGetLastError())
            {
                case WSAETIMEDOUT:
                    break;
                default:
                    dispatch();
            }
        }
    }
}