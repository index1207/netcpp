#include "PCH.h"
#include "IoSystem.hpp"

#include <thread>
#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"
#include "Socket.hpp"

using namespace net;

IoSystem net::ioSystem;

IoSystem::IoSystem()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	for (unsigned i = 0; i < info.dwNumberOfProcessors; ++i)
    {
        _workers.push_back(new std::thread([this] {
            while(true)
                worker();
        }));
    }
}

IoSystem::~IoSystem()
{
	CloseHandle(_hcp);
}

void IoSystem::push(SOCKET s)
{
	if(INVALID_HANDLE_VALUE == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL))
        throw network_error("CreateIoCompletionPort");
}

void IoSystem::worker() {
    Context *context = nullptr;
    ULONG_PTR key = 0;
    DWORD numOfBytes = 0;

    if (GetQueuedCompletionStatus(_hcp,
                                  &numOfBytes,
                                  &key,
                                  reinterpret_cast<LPOVERLAPPED *>(&context),
                                  INFINITE))
    {
        dispatch(context, numOfBytes, true);
    }
    else
    {
        dispatch(context, numOfBytes, false);
    }
    return;
}

void IoSystem::dispatch(Context* context, DWORD numOfBytes, bool isSuccess) {
    if(context == nullptr)
        return;
    switch (context->_contextType) {
        case ContextType::Accept:
            ioSystem.push(context->acceptSocket->getHandle());
            context->acceptSocket->setSocketOption(OptionLevel::Socket, (OptionName)SO_UPDATE_ACCEPT_CONTEXT, _listeningSocket->getHandle());
            break;
        case ContextType::Connect:
            context->acceptSocket->setSocketOption(OptionLevel::Socket, (OptionName)SO_UPDATE_CONNECT_CONTEXT, nullptr);
            break;
        case ContextType::Disconnect:
            break;
        case ContextType::Receive:
        case ContextType::Send:
            context->length.store(numOfBytes);
            break;
        default:
            break;
    }
    context->completed(context, isSuccess);
}
